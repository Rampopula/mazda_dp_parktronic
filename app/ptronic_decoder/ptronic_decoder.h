/**
 * @file       ptronic_decoder.h
 * @brief      Parktronic decoder API wrapper, the purpose is
 *             to use different car parktronic systems.
 *
 * @date       April 29, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_DECODER_H__
#define __MDP_DECODER_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "boardinfo.h"

#define MDP_SENSOR_CNT 0

#if defined(MDP_PTRONIC_F2616)
#include "falcon2616.h"

#undef MDP_SENSOR_CNT
#define MDP_SENSOR_CNT F2616_SNS_CNT

#define MDP_SENSOR_A F2616_SNS_A
#define MDP_SENSOR_B F2616_SNS_B
#define MDP_SENSOR_C F2616_SNS_C
#define MDP_SENSOR_D F2616_SNS_D
#else
#error "Parktronic is not specified!"
#endif

struct ptronic_data {
	bool valid;
	uint32_t distance[MDP_SENSOR_CNT]; /* Distance in centimeters */
};

static inline bool ptronic_ready(void)
{
#ifdef MDP_PTRONIC_F2616
	return f2616_ready();
#endif
	return false;
}

static inline struct ptronic_data ptronic_read_data(void)
{
#ifdef MDP_PTRONIC_F2616
	struct f2616_distance f2616_distance;
#endif
	struct ptronic_data ptronic_data;

	ptronic_data.valid = false;
#ifdef MDP_PTRONIC_F2616
	f2616_distance = f2616_read_distance();
	ptronic_data.valid = f2616_distance.valid_data;
	memcpy(ptronic_data.distance, f2616_distance.distance_cm,
	       sizeof(f2616_distance.distance_cm));
#endif

	return ptronic_data;
}

#endif /* __MDP_DECODER_H__ */
