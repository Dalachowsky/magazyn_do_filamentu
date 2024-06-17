
#ifndef __FILAMUX_IO_
#define __FILAMUX_IO_

#include <stdbool.h>

enum mux_sensor_type {
	IO_MUX_IN,
	IO_MUX_MID,
	IO_MUX_OUT,
};

void io_init();
bool io_get_mux_out();
bool io_get_mux_common();
bool io_get_mux(unsigned int output, enum mux_sensor_type);
bool io_get_cam(unsigned int output);

#endif /* __FILAMUX_IO_ */
