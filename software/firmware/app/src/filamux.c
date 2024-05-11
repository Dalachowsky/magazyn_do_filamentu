
#include "filamux.h"

int filamux_init(struct filamux **o)
{
	*o = malloc(sizeof(struct filamux));
	filamux_fsm_init(*o);
	new_filament_counter(&(*o)->counter, 2);
	new_filament_drive(&(*o)->drive, 2);
	return 0;
}
