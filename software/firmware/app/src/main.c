/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "filamux_counter.h"
#include "filamux.h"
#include "filamux_serial.h"
#include "filamux_io.h"
#include "state_machine.h"

void extruder_demo(struct filamux *filamux)
{
	filamux_extruder_set_speed(filamux->extruder, 1.0);
	k_sleep(K_MSEC(3000));
	LOG_INF("Feeding 10mm");
	filamux_extruder_feed(filamux->extruder, 100);
	k_sleep(K_MSEC(3000));
	LOG_INF("Feeding -5mm");
	filamux_extruder_set_speed(filamux->extruder, 0.5);
	filamux_extruder_feed(filamux->extruder, -50);
	k_sleep(K_MSEC(3000));
	LOG_INF("Feeding 20mm");
	filamux_extruder_set_speed(filamux->extruder, 2.0);
	filamux_extruder_feed(filamux->extruder, 200);
}

void drive_demo(struct filamux *filamux)
{
	LOG_INF("Setting drive to 1");
	set_active_filament_drive(filamux->drive, 1);
	set_filament_drive_feed(filamux->drive, 50);
	k_sleep(K_MSEC(1000));
	set_filament_drive_feed(filamux->drive, 100);
	k_sleep(K_MSEC(1000));
	set_filament_drive_feed(filamux->drive, -50);
	k_sleep(K_MSEC(1000));
	LOG_INF("Setting drive to 2");
	set_active_filament_drive(filamux->drive, 2);
	set_filament_drive_feed(filamux->drive, 50);
	k_sleep(K_MSEC(1000));
	set_filament_drive_feed(filamux->drive, 100);
	k_sleep(K_MSEC(1000));
	set_filament_drive_feed(filamux->drive, -50);
	k_sleep(K_MSEC(1000));
}

int main(void)
{
	int ret;

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

	struct filamux *filamux = NULL;
	filamux_init(&filamux);
	LOG_INF("Initializing serial");
	filamux_serial_init(filamux);
	LOG_INF("Initializing io");
	io_init();
	set_filament_drive_feed(filamux->drive, 0);
	filamux_counter_set_current_spool(filamux->counter, 1);

	while (1) {
		filamux_fsm_run(filamux);
		// filamux_extruder_feed_continuous(filamux->extruder, -5);
		// drive_demo(filamux);
		//  extruder_demo(filamux);
		filamux_serial_process(filamux);
		k_msleep(100);

		/* Counting */
		filamux_counter_add(filamux->counter,
				    -filamux_extruder_pop_traveled_mm(filamux->extruder));
	}

	return 0;
}
