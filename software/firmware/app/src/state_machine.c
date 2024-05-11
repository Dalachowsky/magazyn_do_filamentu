
#include "state_machine.h"
#include <zephyr/smf.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(fsm, CONFIG_APP_LOG_LEVEL);

const char *main_fsm_state_names[] = {"IDLE", "ERROR", "FILAMENT_RUNOUT", "FILAMENT_EXCHANGE"};
const char *xchange_fsm_state_names[] = {
	"DRIVE_SET_CURRENT", "BACK_OFF", "BACK_OFF_SLOW", "DRIVE_SET_TARGET",
	"FEED_SLOW",         "FEED",     "DRIVE_RESET",
};

struct xchange_fsm_object {
	struct smf_ctx ctx;
	struct filamux *filamux;

	enum xchange_fsm_state current_state;

	int current_filament;
	int target_filament;
};

struct main_fsm_object {
	struct smf_ctx ctx;
	struct filamux *filamux;

	enum main_fsm_state current_state;

	struct xchange_fsm_object xchange_fsm;
};

static const struct smf_state main_fsm_states[];
static const struct smf_state xchange_fsm_states[];
static void set_main_fsm_state(struct main_fsm_object *o, enum main_fsm_state new_state);
static void set_xchange_fsm_state(struct xchange_fsm_object *o, enum xchange_fsm_state new_state);

/* ##########################################
 *
 * Main FSM state implementations
 *
 * ##########################################
 */

static void state_idle_entry(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_idle_exit(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_idle_run(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
	set_main_fsm_state(o, STATE_ERROR);
}

static void state_error_entry(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_error_exit(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_error_run(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_runout_entry(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_runout_exit(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_runout_run(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_exchange_entry(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_exchange_exit(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

static void state_filament_exchange_run(void *o)
{
	struct main_fsm_object *fsm = (struct main_fsm_object *)o;
}

/* ##########################################
 *
 * Filament exchange FSM state implementations
 *
 * ##########################################
 */
static void statex_drive_set_current_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_set_current_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_set_current_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_back_off_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
	set_filament_drive_feed(fsm->filamux->drive, 50);
}

static void statex_back_off_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_back_off_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
	if (!MUX_OUT()) {
		set_xchange_fsm_state(fsm, STATEX_BACK_OFF_SLOW);
	}
}

static void statex_back_off_slow_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_back_off_slow_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_back_off_slow_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_set_target_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_set_target_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_set_target_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_slow_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_slow_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_slow_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_feed_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_reset_entry(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_reset_exit(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

static void statex_drive_reset_run(void *o)
{
	struct xchange_fsm_object *fsm = (struct xchange_fsm_object *)o;
}

/* ##########################################
 *
 * State machine definitions
 *
 * ##########################################
 */

static const struct smf_state main_fsm_states[] = {
	[STATE_IDLE] = SMF_CREATE_STATE(state_idle_entry, state_idle_run, state_idle_exit),
	[STATE_ERROR] = SMF_CREATE_STATE(state_error_entry, state_error_run, state_error_exit),
	[STATE_FILAMENT_RUNOUT] = SMF_CREATE_STATE(
		state_filament_runout_entry, state_filament_runout_run, state_filament_runout_exit),
	[STATE_FILAMENT_EXCHANGE] =
		SMF_CREATE_STATE(state_filament_exchange_entry, state_filament_exchange_run,
				 state_filament_exchange_exit),
};

static const struct smf_state xchange_fsm_states[] = {
	[STATEX_DRIVE_SET_CURRENT] =
		SMF_CREATE_STATE(statex_drive_set_current_entry, statex_drive_set_current_run,
				 statex_drive_set_current_exit),
	[STATEX_BACK_OFF] =
		SMF_CREATE_STATE(statex_back_off_entry, statex_back_off_run, statex_back_off_exit),
	[STATEX_BACK_OFF_SLOW] = SMF_CREATE_STATE(
		statex_back_off_slow_entry, statex_back_off_slow_run, statex_back_off_slow_exit),
	[STATEX_DRIVE_SET_TARGET] =
		SMF_CREATE_STATE(statex_drive_set_target_entry, statex_drive_set_target_run,
				 statex_drive_set_target_exit),
	[STATEX_FEED_SLOW] = SMF_CREATE_STATE(statex_feed_slow_entry, statex_feed_slow_run,
					      statex_feed_slow_exit),
	[STATEX_FEED] = SMF_CREATE_STATE(statex_feed_entry, statex_feed_run, statex_feed_exit),
	[STATEX_DRIVE_RESET] = SMF_CREATE_STATE(statex_drive_reset_entry, statex_drive_reset_run,
						statex_drive_reset_exit),
};

/* ##########################################
 *
 * Functions
 *
 * ##########################################
 */

static void set_main_fsm_state(struct main_fsm_object *o, enum main_fsm_state new_state)
{
	LOG_INF("Main FSM change: [%20s ] => [ %-20s]", main_fsm_state_names[o->current_state],
		main_fsm_state_names[new_state]);
	smf_set_state(SMF_CTX(&o->ctx), &main_fsm_states[new_state]);
	o->current_state = new_state;
}

static void set_xchange_fsm_state(struct xchange_fsm_object *o, enum xchange_fsm_state new_state)
{
	LOG_INF("Exchange FSM change: [%20s ] => [ %-20s]",
		xchange_fsm_state_names[o->current_state], xchange_fsm_state_names[new_state]);
	smf_set_state(SMF_CTX(&o->ctx), &xchange_fsm_states[new_state]);
	o->current_state = new_state;
}

static void reset_xchange_fsm(struct xchange_fsm_object *o)
{
	LOG_INF("Exchange FSM reset");
	set_xchange_fsm_state(o, STATEX_DRIVE_SET_CURRENT);
	o->current_filament = 0;
	o->target_filament = 0;
}

static void start_xchange_fsm(struct xchange_fsm_object *o, int current_filament,
			      int target_filament)
{

	reset_xchange_fsm(o);
}

void filamux_fsm_init(struct filamux *filamux)
{
	struct main_fsm_object *fsm = malloc(sizeof(struct main_fsm_object));
	memset(fsm, 0, sizeof(struct main_fsm_object));

	fsm->current_state = STATE_IDLE;
	fsm->xchange_fsm.current_state = STATEX_DRIVE_SET_CURRENT;
	smf_set_initial(SMF_CTX(fsm), &main_fsm_states[fsm->current_state]);
	smf_set_initial(SMF_CTX(&fsm->xchange_fsm),
			&xchange_fsm_states[fsm->xchange_fsm.current_state]);

	fsm->filamux = filamux;
	fsm->xchange_fsm.filamux = filamux;
	filamux->fsm = fsm;
}

void filamux_fsm_run(struct filamux *filamux)
{
	smf_run_state(SMF_CTX(filamux->fsm));
}
