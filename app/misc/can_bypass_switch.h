/**
 * @file       bypass_switch.h
 * @brief      CAN bypass switch control interface, the purpose is to implement
 *             fallback mechanism. If there is any faults with MCU the board
 *             will bypass CAN bus through and it will prevent the car display
 *             from invalid states.
 *
 * @date       August 20, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_CAN_BYPASS_SWITCH_H__
#define __MDP_CAN_BYPASS_SWITCH_H__

#include "stm32f1xx_hal.h"

#define MDP_CAN_IN_SWITCH_GPIO		GPIOA
#define MDP_CAN_IN_SWITCH_PIN		GPIO_PIN_8

#define MDP_CAN_OUT_SWITCH_GPIO		GPIOB
#define MDP_CAN_OUT_SWITCH_PIN		GPIO_PIN_15

#define MDP_CAN_SWITCH_BYPASS_ON	GPIO_PIN_SET
#define MDP_CAN_SWITCH_BYPASS_OFF	GPIO_PIN_RESET

static inline void mdp_can_bypass_on(void)
{
	HAL_GPIO_WritePin(MDP_CAN_IN_SWITCH_GPIO, MDP_CAN_IN_SWITCH_PIN,
			  MDP_CAN_SWITCH_BYPASS_ON);
	HAL_GPIO_WritePin(MDP_CAN_OUT_SWITCH_GPIO, MDP_CAN_OUT_SWITCH_PIN,
			  MDP_CAN_SWITCH_BYPASS_ON);
}

static inline void mdp_can_bypass_off(void)
{
	HAL_GPIO_WritePin(MDP_CAN_IN_SWITCH_GPIO, MDP_CAN_IN_SWITCH_PIN,
			  MDP_CAN_SWITCH_BYPASS_OFF);
	HAL_GPIO_WritePin(MDP_CAN_OUT_SWITCH_GPIO, MDP_CAN_OUT_SWITCH_PIN,
			  MDP_CAN_SWITCH_BYPASS_OFF);
}

#endif /* __MDP_CAN_BYPASS_SWITCH_H__ */
