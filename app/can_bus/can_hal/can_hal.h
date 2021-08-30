/**
 * @file       can_hal.h
 * @brief      CAN HAL interface wrapper.
 *
 * @date       August 25, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_CAN_HAL_H__
#define __MDP_CAN_HAL_H__

#include "stm32f1xx_hal.h"

int mdp_can_hal_start(void);
int mdp_can_hal_stop(void);
int mdp_can_hal_read(uint32_t *msg_id, uint8_t *data, uint32_t *size);
int mdp_can_hal_write(uint32_t msg_id, uint8_t *data, uint32_t size);

#endif /* __MDP_CAN_HAL_H__*/
