#include "filament_drive.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
LOG_MODULE_REGISTER(drive, CONFIG_APP_LOG_LEVEL);

struct filament_drive_channel {
	struct gpio_dt_spec ph_gpio;
	int pwm_channel;
};

struct filament_drive {
	unsigned int active_drive;
	unsigned int drive_count;

	struct filament_drive_channel *channel_active;
};

#define CHANNEL_DATA(node_id)                                                                      \
	{                                                                                          \
		.ph_gpio = GPIO_DT_SPEC_GET(node_id, phase_gpio),                                  \
		.pwm_channel = DT_PROP(node_id, pwm_channel),                                      \
	},

static struct filament_drive_channel channels[] = {
	DT_FOREACH_CHILD(DT_NODELABEL(filamux_drive), CHANNEL_DATA)};

static const struct device *pwm = DEVICE_DT_GET(DT_NODELABEL(pwm_drive));

int new_filament_drive(struct filament_drive **drive, unsigned int drive_count)
{
	*drive = malloc(sizeof(struct filament_drive));
	(*drive)->active_drive = 0;
	(*drive)->drive_count = sizeof(channels) / sizeof(struct filament_drive_channel);
	(*drive)->channel_active = NULL;
	gpio_pin_configure_dt(&channels[0].ph_gpio, GPIO_OUTPUT | GPIO_PUSH_PULL);
	gpio_pin_configure_dt(&channels[1].ph_gpio, GPIO_OUTPUT | GPIO_PUSH_PULL);
	return 0;
}

int get_active_filament_drive(struct filament_drive *drive)
{
	/* TODO check gpios */
	return drive->active_drive;
}

int set_active_filament_drive(struct filament_drive *drive, int drive_number)
{
	LOG_DBG("Setting filament drive to: %d", drive_number);
	if (drive_number < 0 || drive_number > drive->drive_count) {
		LOG_ERR("Invalid drive number: %d", drive_number);
		return -1;
	}
	if (drive->active_drive != 0) {
		LOG_DBG("Stopping current drive");
		set_filament_drive_feed(drive, 0);
	}
	drive->active_drive = drive_number;
	if (drive_number == 0) {
		drive->channel_active = NULL;
	} else {
		drive->channel_active = &channels[drive_number - 1];
	}
	LOG_DBG("Set active drive to %d", drive_number);
	return 0;
}

int set_filament_drive_feed(struct filament_drive *drive, int feed)
{
	if (drive->active_drive <= 0) {
		LOG_ERR("Cannot set drive feed when no drive is active");
		return -1;
	}
	if (drive->channel_active == NULL) {
		LOG_ERR("Cannot set drive. channel_active == NULL");
		return -1;
	}
	LOG_INF("Setting filament drive feed to %d", feed);
	bool is_reverse = feed < 0;
	unsigned int feed_abs = abs(feed);
	const uint32_t period = 10000;
	uint32_t pulse_width = period;
	if (feed_abs > 100) {
		LOG_ERR("cannot set feed to %d%. Capping to 100%");
	} else {
		LOG_DBG("Set feed to %d", feed);
		pulse_width = (feed_abs / 100.0) * period;
	}
	gpio_pin_set_dt(&drive->channel_active->ph_gpio, is_reverse);
	pwm_set(pwm, drive->channel_active->pwm_channel, period, pulse_width, PWM_POLARITY_NORMAL);
	return 0;
}
