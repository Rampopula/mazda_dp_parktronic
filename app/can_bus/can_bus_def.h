/**
 * @file       can_bus_def.h
 * @brief      CAN Bus common definitions.
 *
 * @date       September 1, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_CAN_BUS_DEF_H__
#define __MDP_CAN_BUS_DEF_H__

#include <stdint.h>

#define MDP_CAN_FRAME_LEN	8

struct mdp_can_msg {
	uint32_t id;
	uint32_t size;
	uint8_t data[MDP_CAN_FRAME_LEN];
};

#endif /* __MDP_CAN_BUS_DEF_H__*/
