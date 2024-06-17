
#include "filamux_extruder.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
LOG_MODULE_REGISTER(extruder, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec step_gpio =
	GPIO_DT_SPEC_GET(DT_NODELABEL(filamux_extruder), step_gpio);
static const struct gpio_dt_spec dir_gpio =
	GPIO_DT_SPEC_GET(DT_NODELABEL(filamux_extruder), dir_gpio);
static const struct device *timer = DEVICE_DT_GET(DT_NODELABEL(counter_extruder));

const int E_STEPS = 415;

enum extruder_mode {
	EXTRUDER_MODE_STEP = 1,  ///< Count set amount of steps
	EXTRUDER_MODE_CONTINUOUS ///< Extruder continuously feeds with set speed
};

struct filamux_extruder {
	bool feeding_forward;
	uint32_t remaining_steps;
	enum extruder_mode mode;

	int speed;
	int steps_buffer; ///< For counting steps before passing 1mm or -1mm
	int mm_travelled; ///< mm traveled since last invocation of pop_traveled_mm
};

static void irq_extruder_timer(const struct device *timer, uint8_t ch, uint32_t ticks,
			       void *user_data)
{
	LOG_INF("Extruder IRQ. Channel: %d Ticks: %d", ch, ticks);
}

static void irq_handler(const struct device *dev, void *user_data)
{
	struct filamux_extruder *extruder = (struct filamux_extruder *)user_data;
	if (extruder->remaining_steps > 0 || extruder->mode == EXTRUDER_MODE_CONTINUOUS) {
		gpio_pin_toggle_dt(&step_gpio);

		if (gpio_pin_get_dt(&step_gpio)) {
			if (extruder->mode == EXTRUDER_MODE_STEP) {
				if (--extruder->remaining_steps <= 0) {
					extruder->speed = 0;
				}
			}

			/* Count steps */
			extruder->steps_buffer += extruder->feeding_forward;
			if (extruder->steps_buffer >= E_STEPS) {
				extruder->steps_buffer = 0;
				extruder->mm_travelled += extruder->feeding_forward;
			}
		}
	}
}

static void set_dir_pin(struct filamux_extruder *extruder, bool state)
{
	extruder->feeding_forward = state;
	gpio_pin_set_dt(&dir_gpio, state);
}

int filamux_extruder_init(struct filamux_extruder **extruder)
{
	*extruder = malloc(sizeof(struct filamux_extruder));
	(*extruder)->remaining_steps = 0;
	(*extruder)->feeding_forward = true;
	(*extruder)->mm_travelled = 0;
	(*extruder)->steps_buffer = 0;
	(*extruder)->mode = EXTRUDER_MODE_STEP;

	if (!gpio_is_ready_dt(&step_gpio)) {
		LOG_ERR("Step pin not ready");
		return -1;
	}
	if (!gpio_is_ready_dt(&dir_gpio)) {
		LOG_ERR("Step pin not ready");
		return -1;
	}
	if (!device_is_ready(timer)) {
		LOG_ERR("Timer not ready");
		return -1;
	}

	gpio_pin_configure_dt(&step_gpio, GPIO_OUTPUT | GPIO_PUSH_PULL);
	gpio_pin_configure_dt(&dir_gpio, GPIO_OUTPUT | GPIO_PUSH_PULL);

	filamux_extruder_set_speed(*extruder, 1);
	counter_start(timer);
	return 0;
}

int filamux_extruder_set_speed(struct filamux_extruder *extruder, float speed)
{
	LOG_DBG("Setting extrusion speed to %.02f", speed);
	extruder->speed = (int)speed;
	float period = 1 / fabs(speed);
	struct counter_top_cfg timer_cfg = {.callback = irq_handler,
					    .flags = 0,
					    .ticks = counter_us_to_ticks(timer, period * 1000),
					    .user_data = extruder};
	counter_set_top_value(timer, &timer_cfg);
	return 0;
}

int filamux_extruder_feed(struct filamux_extruder *extruder, float mm)
{
	LOG_DBG("Feeding %.02f mm", mm);
	extruder->mode = EXTRUDER_MODE_STEP;
	set_dir_pin(extruder, mm > 0);
	extruder->remaining_steps = fabs(mm) * E_STEPS;
	return 0;
}

int filamux_extruder_feed_continuous(struct filamux_extruder *extruder, float speed)
{
	LOG_INF("Extruder feeding continously %.02f", speed);
	extruder->mode = EXTRUDER_MODE_CONTINUOUS;
	set_dir_pin(extruder, speed > 0);
	filamux_extruder_set_speed(extruder, fabs(speed));
}

int filamux_extruder_pop_traveled_mm(struct filamux_extruder *extruder)
{
	int traveled_mm = extruder->mm_travelled;
	extruder->mm_travelled = 0;
	return traveled_mm;
}

int filamux_extruder_get_speed(struct filamux_extruder *extruder)
{
	return extruder->speed;
}
