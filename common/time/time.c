/**
 * @file       time.c
 * @brief      Time measurement API implementation.
 *
 * @date       August 31, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#include "time.h"
#include "boardinfo.h"

#include <stdbool.h>

#define ARM_CM_DEMCR		(*(uint32_t *)0xE000EDFC)
#define ARM_CM_DWT_CTRL		(*(uint32_t *)0xE0001000)
#define ARM_CM_DWT_CYCCNT	(*(volatile uint32_t *)0xE0001004)

static bool _inited = false;

static inline void time_init(void)
{
	if (_inited && ARM_CM_DWT_CTRL)
		return;

	ARM_CM_DEMCR |= 1 << 24;
	ARM_CM_DWT_CYCCNT = 0;
	ARM_CM_DWT_CTRL |= 1 << 0;

	_inited = true;
}

void mdp_tm_measure_start(struct mdp_time *tm)
{
	if (!_inited)
		time_init();

	tm->start = ARM_CM_DWT_CYCCNT;
}

void mdp_tm_measure_stop(struct mdp_time *tm)
{
	tm->cycles = ARM_CM_DWT_CYCCNT - tm->start;
}

uint32_t mdp_tm_measure_get_us(struct mdp_time *tm)
{
	return tm->cycles / MDP_CLOCK_FREQ_MHZ;
}

void mdp_tm_msleep(uint32_t msecs)
{
	HAL_Delay(msecs);
}

bool mdp_tm_elapsed(struct mdp_timestamp *ts, uint32_t interval_ms)
{
	if (MDP_TIMESTAMP.ticks - ts->ticks >= interval_ms) {
		*ts = MDP_TIMESTAMP;
		return true;
	}

	return false;
}
