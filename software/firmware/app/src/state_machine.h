

#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_

#include "filamux.h"

/* Main state machine */
enum main_fsm_state {
	STATE_IDLE,              ///< No operation
	STATE_ERROR,             ///< System error
	STATE_FILAMENT_RUNOUT,   ///< Filament has run out. Waiting for refill
	STATE_FILAMENT_EXCHANGE, ///< Exchange process in progress
	_STATE_COUNT
};

/* Filament exchange state machine */
enum xchange_fsm_state {
	STATEX_DRIVE_SET_CURRENT, ///< Prepare drive to back off
	STATEX_BACK_OFF,          ///< Back off filament from extruder
	STATEX_BACK_OFF_SLOW,     ///< Slowly back off filament from output channel
	STATEX_DRIVE_SET_TARGET,  ///< Prepare drive to feed new filament
	STATEX_FEED_SLOW,         ///< Slowly feed new filament out of output channel
	STATEX_FEED,              ///< Feed filament to extruder
	STATEX_DRIVE_RESET,       ///< Reset drive
	_STATEX_COUNT
};

extern const char *main_fsm_state_names[_STATE_COUNT];
extern const char *xchange_fsm_state_names[_STATEX_COUNT];

struct main_fsm_object;

void filamux_fsm_init(struct filamux *filamux);

/**
 * Run FSM state
 */
void filamux_fsm_run(struct filamux *filamux);

#endif /* __STATE_MACHINE_H_ */
