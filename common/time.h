/**
 * @file       time.h
 * @brief      Simple helper functions for working with time.
 *
 * @date       August 17, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_TIME_H__
#define __MDP_TIME_H__

#include "stm32f1xx_hal.h"

struct mdp_timestamp {
	uint32_t sec;
	uint32_t msec;
};

#define __MDP_SEC_SHIFT 10 /* Will consider 1 sec is 1024 msec */
#define __MDP_MSEC_IN_SEC 1000

#define MDP_TIMESTAMP() \
	({ \
		struct mdp_timestamp __ts; \
		uint32_t __ticks = HAL_GetTick(); \
		__ts.sec = __ticks >> __MDP_SEC_SHIFT; \
		__ts.msec = __ticks - (__ts.sec << __MDP_SEC_SHIFT); \
		if (__ts.msec >= __MDP_MSEC_IN_SEC) { \
			__ts.msec -= __MDP_MSEC_IN_SEC; \
			__ts.sec++; \
		} \
		__ts; \
	})

#endif /* __MDP_TIME_H__ */
