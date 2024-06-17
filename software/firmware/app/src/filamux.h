
#include "state_machine.h"
#include "filamux_counter.h"
#include "filament_drive.h"
#include "filamux_extruder.h"

#ifndef __FILAMUX_H_
#define __FILAMUX_H_

struct filamux {
	struct filament_counter *counter;
	struct filament_drive *drive;
	struct filamux_extruder *extruder;
	struct main_fsm_object *fsm;

	unsigned int target_spool;

	struct {
		int distance;
		int speed;
	} extruder_req;
};

int filamux_init(struct filamux **o);

/**
 * @brief Set target spool
 * @returns 0 on success
 * @returns -ERANGE if spool out of range
 */
int filamux_set_target_spool(struct filamux *o, unsigned int target_spool);

/**
 * @brief Request extruder feed
 * @returns 0 on success
 */
int filamux_feed_extruder(struct filamux *o, int speed, int distance);

#endif /* __FILAMUX_H_ */
