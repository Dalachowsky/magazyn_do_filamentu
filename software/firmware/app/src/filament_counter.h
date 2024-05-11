
#ifndef __FILAMENT_COUNTER_H_
#define __FILAMENT_COUNTER_H_

struct filament_counter;

/**
 * Allocate new filament counter
 */
int new_filament_counter(struct filament_counter **counter, unsigned int spools_count);

/**
 * Get used length of current filament spool
 * @returns -1 if no spool selected
 */
int get_current_filament_used(struct filament_counter *counter);

/**
 * Get used length of specified filament spool
 * @param index index of filament spool to fetch
 */
int get_filament_used(struct filament_counter *counter, unsigned int index);

/**
 * Set current spool to index
 * @param index which spool is used now
 */
int set_current_spool(struct filament_counter *counter, unsigned int index);

/**
 * Get current spool
 * @returns 0 if no spool selected
 */
int get_current_spool(struct filament_counter *counter);

#endif /* __FILAMENT_COUNTER_H_ */
