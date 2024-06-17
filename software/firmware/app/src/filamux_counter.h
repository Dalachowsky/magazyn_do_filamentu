
#ifndef __FILAMUX_COUNTER_H_
#define __FILAMUX_COUNTER_H_

struct filamux_counter;

/**
 * Allocate new filament counter
 */
int new_filamux_counter(struct filamux_counter **counter, unsigned int spools_count);

/**
 * Get length of current filament spool
 * @returns -1 if no spool selected
 */
int filamux_counter_get_current_filament_length(struct filamux_counter *counter);

/**
 * Get length of specified filament spool
 * @param index index of filament spool to fetch
 */
int filamux_counter_get_filament_length(struct filamux_counter *counter, unsigned int index);

/**
 * Set current spool to index
 * @param index which spool is used now
 */
int filamux_counter_set_current_spool(struct filamux_counter *counter, unsigned int index);

/**
 * Get current spool
 * @returns 0 if no spool selected
 */
int filamux_counter_get_current_spool(struct filamux_counter *counter);

/**
 * Add value to current spool length
 */
int filamux_counter_add(struct filamux_counter *counter, int value);

#endif /* __FILAMUX_COUNTER_H_ */
