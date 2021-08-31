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

#include <stdint.h>
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#define __MDP_SEC_SHIFT 	10	/* Will consider 1 sec is 1024 msec */
#define __MDP_MSEC_IN_SEC	1000

#define MDP_TIMESTAMP \
	({ \
		struct mdp_timestamp __ts; \
		__ts.ticks = HAL_GetTick(); \
		__ts.sec = __ts.ticks >> __MDP_SEC_SHIFT; \
		__ts.msec = __ts.ticks - (__ts.sec << __MDP_SEC_SHIFT); \
		if (__ts.msec >= __MDP_MSEC_IN_SEC) { \
			__ts.msec -= __MDP_MSEC_IN_SEC; \
			__ts.sec++; \
		} \
		__ts; \
	})

struct mdp_time {
	uint32_t start;
	uint32_t cycles;
};

struct mdp_timestamp {
	uint32_t sec;
	uint32_t msec;
	uint32_t ticks;
};

/**
 * @brief Start measuring time.
 *        Call this function before the piece of code
 *        to measure its execution time.
 *
 * @param [in] tm Valid time context.
 */
void mdp_tm_measure_start(struct mdp_time *tm);

/**
 * @brief Stop measuring time.
 *        Call this function after the piece of code
 *        to measure its execution time.
 *
 * @param [in] tm Valid time context.
 */
void mdp_tm_measure_stop(struct mdp_time *tm);

/**
 * @brief Get execution time in microseconds.
 *        For this function to work, the MDP_CLOCK_FREQ_MHZ must be defined.
 *        If the MDP_CLOCK_FREQ_MHZ is not defined, the default value
 *        of 16 MHz will be taken.
 *
 * @param [in] tm Valid time context.
 *
 * @return Time in microseconds.
 */
uint32_t mdp_tm_measure_get_us(struct mdp_time *tm);

/**
 * @brief Perform millisecond sleep.
 *
 * @param [in] msecs Sleep time in milliseconds.
 */
void mdp_tm_msleep(uint32_t msecs);

/**
 * @brief Check if the time interval has elapsed.
 *
 * @param [in, out] ts Valid timestamp context.
 * @param [in] interval_ms Time interval to check.
 *
 * @return Time in microseconds.
 */
bool mdp_tm_elapsed(struct mdp_timestamp *ts, uint32_t interval_ms);

#endif /* __MDP_TIME_H__ */
