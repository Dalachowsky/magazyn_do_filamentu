
#include "filament_counter.h"
#include <stdint.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(counter, CONFIG_APP_LOG_LEVEL);

struct spool {
	unsigned int index;
	uint32_t used_length; ///< In mm
};

struct filament_counter {
	struct spool *current_spool; ///< Current spool. NULL means no spool.
	unsigned int spool_count;    ///< Number of spools
	struct spool *spools;
};

int new_filament_counter(struct filament_counter **counter, unsigned int spools_count)
{
	*counter = malloc(sizeof(struct filament_counter));
	if (!counter) {
		LOG_ERR("Could not allocate filament counter");
		return -1;
	}
	(*counter)->current_spool = NULL;
	(*counter)->spools = malloc(sizeof(struct spool[spools_count]));
	if (!(*counter)->spools) {
		LOG_ERR("Could not allocate spool buffers");
		return -1;
	}
	(*counter)->spool_count = spools_count;

	struct spool *spool = (*counter)->spools;
	for (int i = 0; i < spools_count; i++) {
		spool->index = i + 1;
		spool->used_length = 0;
		spool++;
	}

	return 0;
}

int get_current_filament_used(struct filament_counter *counter)
{
	if (!counter->current_spool) {
		LOG_ERR("No spool selected. Cannot get used length");
		return -1;
	}
	return counter->current_spool->used_length;
}

int get_filament_used(struct filament_counter *counter, unsigned int index)
{
	if (index > counter->spool_count || index == 0) {
		LOG_ERR("Invalid spool index: %d", index);
		return -1;
	}
	return counter->spools[index - 1].used_length;
}

int set_current_spool(struct filament_counter *counter, unsigned int index)
{
	if (index > counter->spool_count || index == 0) {
		LOG_ERR("Invalid spool index: %d", index);
		return -1;
	}
	counter->current_spool = &counter->spools[index - 1];
	LOG_INF("Current spool set to %d", index);
	return 0;
}

int get_current_spool(struct filament_counter *counter)
{
	if (!counter->current_spool) {
		return 0;
	}
	return counter->current_spool->index;
}
