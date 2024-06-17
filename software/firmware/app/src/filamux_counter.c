
#include "filamux_counter.h"
#include <stdint.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
LOG_MODULE_REGISTER(counter, CONFIG_APP_LOG_LEVEL);

struct spool {
	unsigned int index;
	int length; ///< In mm
};

struct filamux_counter {
	struct spool *current_spool; ///< Current spool. NULL means no spool.
	unsigned int spool_count;    ///< Number of spools
	struct spool *spools;
};

int new_filament_counter(struct filamux_counter **counter, unsigned int spools_count)
{
	*counter = malloc(sizeof(struct filamux_counter));
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
		spool->length = 0;
		spool++;
	}

	return 0;
}

int filamux_counter_get_current_filament_length(struct filamux_counter *counter)
{
	if (!counter->current_spool) {
		LOG_ERR("No spool selected. Cannot get used length");
		return -1;
	}
	return counter->current_spool->length;
}

int filamux_counter_get_filament_length(struct filamux_counter *counter, unsigned int index)
{
	if (index > counter->spool_count || index == 0) {
		LOG_ERR("Invalid spool index: %d", index);
		return -1;
	}
	return counter->spools[index - 1].length;
}

int filamux_counter_set_current_spool(struct filamux_counter *counter, unsigned int index)
{
	if (index > counter->spool_count || index == 0) {
		LOG_ERR("Invalid spool index: %d", index);
		return -1;
	}
	counter->current_spool = &counter->spools[index - 1];
	LOG_INF("Current spool set to %d", index);
	return 0;
}

int filamux_counter_get_current_spool(struct filamux_counter *counter)
{
	if (!counter->current_spool) {
		return 0;
	}
	return counter->current_spool->index;
}

int filamux_counter_add(struct filamux_counter *counter, int value)
{
	if (!counter->current_spool) {
		LOG_ERR("Cannot add to spool length. No spool selected");
		return -1;
	}
	counter->current_spool->length += value;

	return 0;
}
