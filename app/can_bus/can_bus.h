/**
 * @file       can_bus.h
 * @brief      CAN Bus wrapper for CAN HAL/SPI interfaces.
 *
 * @date       August 25, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_CAN_BUS_H__
#define __MDP_CAN_BUS_H__

#include "can_bus_def.h"
#include "can_sniffer.h"

struct mdp_can_ops {
	int(*start)(void);
	int(*stop)(void);
	int(*read)(uint32_t *, uint8_t *, uint32_t *);
	int(*write)(uint32_t, uint8_t *, uint32_t);
};

struct mdp_can {
	struct mdp_can_msg msg;
	struct mdp_can_ops ops;
	struct mdp_can_sniffer *sniffer;
};

int mdp_can_start(struct mdp_can *can);
int mdp_can_stop(struct mdp_can *can);
int mdp_can_read(struct mdp_can *can);
int mdp_can_write(struct mdp_can *can);

int mdp_can_attach_sniffer(struct mdp_can *can,
			   struct mdp_can_sniffer *sniffer);

struct mdp_can mdp_get_can_hal_interface(void);
struct mdp_can mdp_get_can_spi_interface(void);

#endif /* __MDP_CAN_BUS_H__*/
