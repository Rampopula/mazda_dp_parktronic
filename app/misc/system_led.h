/**
 * @file       system_led.h
 * @brief      System LED control interface.
 *
 * @date       August 18, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_SYSTEM_LED_H__
#define __MDP_SYSTEM_LED_H__

#include "stm32f1xx_hal.h"

#define MDP_SYSLED_GPIO		GPIOC
#define MDP_SYSLED_GPIO_PIN	GPIO_PIN_13

static inline void mdp_sysled_on(void)
{
	HAL_GPIO_WritePin(MDP_SYSLED_GPIO, MDP_SYSLED_GPIO_PIN, GPIO_PIN_RESET);
}

static inline void mdp_sysled_off(void)
{
	HAL_GPIO_WritePin(MDP_SYSLED_GPIO, MDP_SYSLED_GPIO_PIN, GPIO_PIN_SET);
}

static inline void mdp_sysled_toggle(void)
{
	HAL_GPIO_TogglePin(MDP_SYSLED_GPIO, MDP_SYSLED_GPIO_PIN);
}

#endif /* __MDP_PTRONIC_SWITCH_H__ */
