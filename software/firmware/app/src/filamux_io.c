
#include "filamux_io.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec mux_out = GPIO_DT_SPEC_GET(DT_NODELABEL(filamux_io), mux_out_gpio);

void io_init()
{
	gpio_pin_configure_dt(&mux_out, GPIO_ACTIVE_HIGH);
}

bool io_get_mux_out()
{
	return gpio_pin_get_dt(&mux_out);
}

bool io_get_mux_common()
{
	return false;
}

bool io_get_mux(unsigned int output, enum mux_sensor_type)
{
	return false;
}

bool io_get_cam(unsigned int output)
{
	return false;
}
