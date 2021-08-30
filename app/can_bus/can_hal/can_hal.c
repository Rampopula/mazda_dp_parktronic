#include "can_hal.h"
#include "common.h"
#include "log.h"

#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "can_hal"

int mdp_can_hal_start(void)
{
	return -ENOTSUP;
}

int mdp_can_hal_stop(void)
{
	return -ENOTSUP;
}

int mdp_can_hal_read(uint32_t *msg_id, uint8_t *data, uint32_t *size)
{
	return -ENOTSUP;
}

int mdp_can_hal_write(uint32_t msg_id, uint8_t *data, uint32_t size)
{
	return -ENOTSUP;
}
