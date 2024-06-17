
#ifndef __FILAMUX_EXTRUDER_H_
#define __FILAMUX_EXTRUDER_H_

struct filamux_extruder;

int filamux_extruder_init(struct filamux_extruder **extruder);

/**
 * Set extrusion speed
 * @param speed mm/s
 */
int filamux_extruder_set_speed(struct filamux_extruder *extruder, float speed);

/**
 * Feed set amount of milimeters
 */
int filamux_extruder_feed(struct filamux_extruder *extruder, float mm);

/**
 * Start extruding with set speed
 */
int filamux_extruder_feed_continuous(struct filamux_extruder *extruder, float speed);

/**
 * Get travelled mm and clear internal buffer
 * @returns number of mm travelled since last invocation of this function
 */
int filamux_extruder_pop_traveled_mm(struct filamux_extruder *extruder);

/**
 * Get current speed of extruder
 * @returns current speed in mm/s
 */
int filamux_extruder_get_speed(struct filamux_extruder *extruder);

#endif /* __FILAMUX_EXTRUDER_H_ */
