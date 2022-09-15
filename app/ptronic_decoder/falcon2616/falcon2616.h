/**
 * @file       falcon2616.h
 * @brief      Falcon 2616 Parktronic decoder API.
 *
 * Falcon 2616 Data frame diagram.
 *
 * Frame consists of 1 start bit and 16 bits of data.
 * Frame frequency is around 10 Hz (80..100 ms).
 *
 * ¦  start
 * ¦   bit        1   1   1   1   1   1   1   1   0  0   1   0  0   1   0  1
 * ¦  ┌────┐     ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌┐  ┌┐ ┌─┐ ┌┐  ┌┐ ┌─┐ ┌┐ ┌─┐
 * ¦  │    │     │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ ││  ││ │ │ ││  ││ │ │ ││ │ │
 * ¦  │    │     │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ ││  ││ │ │ ││  ││ │ │ ││ │ │
 * ¦  │    │     │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ ││  ││ │ │ ││  ││ │ │ ││ │ │
 * ¦  │    │     │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ ││  ││ │ │ ││  ││ │ │ ││ │ │
 * ¦  │    │     │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ ││  ││ │ │ ││  ││ │ │ ││ │ │
 * ¦──┘    └─────┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘└──┘└─┘ └─┘└──┘└─┘ └─┘└─┘ └─
 * ├--┬----┬-----┬-------┬-┬-----------┬-┬-------┬┬----------┬--┬-----------┬-->
 * │  │    │     │       │ │           │ │       ││          │  │           │ t,
 * │  │<-->│     │       │-│           │-│       ││          │--│           │ us
 * │   800-│     │     450-600       240-250   225-235      500-600         │
 * │   900 │<--->│                                                          │
 * │      1700-1850                                                         │
 * │<---------------------------------------------------------------------->│
 * │                  All data frame time is around 15 ms.                  │
 *
 * Frame description (first bit is rightmost on diagram):
 *       bit[15..12] Unknown, not used.
 *       bit[11..10] Sensor status:
 *              1 - OK;
 *              2 - Not connected or obstacle is too far;
 *              Other values - unknown.
 *       bit[8..9] Sensor identifier:
 *              0 - Sensor A;
 *              1 - Sensor D;
 *              2 - Sensor C;
 *              3 - Sensor B.
 *       bit[7..0] Distance:
 *              255 - 0.0m;
 *              252..246 - 0.3m..0.9m;
 *              239..230 - 1.0m..1.9m;
 *              223..218 - 2.0m..2.5m.
 *
 * @date       April 29, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_F2616_DECODER_H__
#define __MDP_F2616_DECODER_H__

#include <stdint.h>
#include <stdbool.h>

#define F2616_SNS_CNT	4

#define F2616_SNS_A	0
#define F2616_SNS_B	3
#define F2616_SNS_C	2
#define F2616_SNS_D	1

struct f2616_distance {
	bool valid_data;
	uint32_t cm[F2616_SNS_CNT];
};

bool f2616_ready(void);

struct f2616_distance *f2616_read_distance(void);

#endif /* __MDP_F2616_DECODER_H__ */
