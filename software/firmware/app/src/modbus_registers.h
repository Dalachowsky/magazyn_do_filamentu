
#ifndef __MODBUS_REGISTERS_H_
#define __MODBUS_REGISTERS_H_

#include "state_machine.h"
#include <stdint.h>

struct input_registers {
    enum main_fsm_state state;
    uint16_t current_filament;
};

#endif /* __MODBUS_REGISTERS_H_ */
