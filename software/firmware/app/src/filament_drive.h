
#ifndef __FILAMENT_DRIVE_H_
#define __FILAMENT_DRIVE_H_

#include "filamux.h"

struct filament_drive;

/**
 * Check which filament drive is active
 * @returns active drive number. 0 if no drive active.
 */
int get_active_filament_drive(struct filament_drive *drive);

/**
 * Set active filament drive
 * @param drive_number number of drive to be activated. 0 to disable actieve drive.
 */
int set_active_filament_drive(struct filament_drive *drive, int drive_number);

/**
 * Set filament drive feed speed.
 * @param feed 0-100% of speed.
 */
int set_filament_drive_feed(struct filament_drive *drive, unsigned int feed);

#endif /* __FILAMENT_DRIVE_H_ */
