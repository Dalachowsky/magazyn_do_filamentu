
#include "state_machine.h"
#include "filament_counter.h"
#include "filament_drive.h"

#ifndef __FILAMUX_H_
#define __FILAMUX_H_

struct filamux {
	struct filament_counter *counter;
	struct filament_drive *drive;
	struct main_fsm_object *fsm;
};

int filamux_init(struct filamux **o);

#endif /* __FILAMUX_H_ */
