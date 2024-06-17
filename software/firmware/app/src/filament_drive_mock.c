#include "filament_drive.h"
#include <zephyr/logging/log.h>
#include <stdlib.h>
LOG_MODULE_REGISTER(drive, CONFIG_APP_LOG_LEVEL);

struct filament_drive {
	unsigned int active_drive;
	unsigned int drive_count;
};

int new_filament_drive(struct filament_drive **drive, unsigned int drive_count)
{
	*drive = malloc(sizeof(struct filament_drive));
	(*drive)->active_drive = 0;
	(*drive)->drive_count = drive_count;
	return 0;
}

int get_active_filament_drive(struct filament_drive *drive)
{
	return drive->active_drive;
}

int set_active_filament_drive(struct filament_drive *drive, int drive_number)
{
	if (drive_number == 0 || drive_number > drive->drive_count) {
		LOG_ERR("Invalid drive number: %d", drive_number);
		return -1;
	}
	drive->active_drive = drive_number;
	LOG_DBG("Set active drive to %d", drive_number);
	return 0;
}

int set_filament_drive_feed(struct filament_drive *drive, int feed)
{
	LOG_DBG("Set feed to %d", feed);
	return 0;
}
