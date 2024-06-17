
#include "filamux.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(core, CONFIG_APP_LOG_LEVEL);

#define SPOOLS_COUNT 2

int filamux_init(struct filamux **o)
{
	*o = malloc(sizeof(struct filamux));
	new_filament_counter(&(*o)->counter, SPOOLS_COUNT);
	new_filament_drive(&(*o)->drive, SPOOLS_COUNT);
	filamux_extruder_init(&(*o)->extruder);
	filamux_fsm_init(*o);

	/* TODO set target spool to current from IO */
	(*o)->target_spool = 0;

	(*o)->extruder_req.distance = 0;
	(*o)->extruder_req.speed = 0;
	return 0;
}

int filamux_set_target_spool(struct filamux *o, unsigned int target_spool)
{
	if (target_spool > SPOOLS_COUNT) {
		LOG_ERR("Cannot set target spool to: %d. Out of range");
		return -ERANGE;
	}
	o->target_spool = target_spool;
	LOG_INF("Set target spool to %d", o->target_spool);
	return 0;
}

int filamux_feed_extruder(struct filamux *o, int speed, int distance)
{
	o->extruder_req.speed = speed;
	o->extruder_req.distance = distance;
	return 0;
}
