/**
 * @file       can_bypass_switch.c
 * @brief      CAN bypass switch control interface implementation.
 *
 * @date       September 21, 2022
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#include "can_bypass_switch.h"

#define MDP_CAN_IN_SWITCH_GPIO		GPIOA
#define MDP_CAN_IN_SWITCH_PIN		GPIO_PIN_8

#define MDP_CAN_OUT_SWITCH_GPIO		GPIOB
#define MDP_CAN_OUT_SWITCH_PIN		GPIO_PIN_15

void mdp_can_bypass_set(can_bypass_state_t state)
{
	HAL_GPIO_WritePin(MDP_CAN_IN_SWITCH_GPIO, MDP_CAN_IN_SWITCH_PIN,
			  state);
	HAL_GPIO_WritePin(MDP_CAN_OUT_SWITCH_GPIO, MDP_CAN_OUT_SWITCH_PIN,
			  state);
}

can_bypass_state_t mdp_can_bypass_get(void)
{
	int in_sw = HAL_GPIO_ReadPin(MDP_CAN_IN_SWITCH_GPIO,
				     MDP_CAN_IN_SWITCH_PIN);
	int out_sw = HAL_GPIO_ReadPin(MDP_CAN_OUT_SWITCH_GPIO,
				      MDP_CAN_OUT_SWITCH_PIN);
	return (can_bypass_state_t)(in_sw && out_sw);
}
