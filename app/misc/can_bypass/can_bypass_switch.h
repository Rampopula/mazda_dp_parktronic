/**
 * @file       can_bypass_switch.h
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

#define mdp_can_bypass_off()	mdp_can_bypass_set(MDP_CAN_BYPASS_OFF)
#define mdp_can_bypass_on()	mdp_can_bypass_set(MDP_CAN_BYPASS_ON)

typedef enum mdp_can_bypass_state {
	MDP_CAN_BYPASS_OFF	= GPIO_PIN_RESET,
	MDP_CAN_BYPASS_ON	= GPIO_PIN_SET,
} can_bypass_state_t;

void mdp_can_bypass_set(can_bypass_state_t state);

can_bypass_state_t mdp_can_bypass_get(void);

#endif /* __MDP_CAN_BYPASS_SWITCH_H__ */
