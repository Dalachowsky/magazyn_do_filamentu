/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <app_version.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#include "filamux.h"
#include "state_machine.h"

int main(void)
{
	int ret;

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

	struct filamux *filamux = NULL;
	filamux_init(&filamux);

	while (1) {
		filamux_fsm_run(filamux);
		k_sleep(K_MSEC(500));
	}

	return 0;
}
