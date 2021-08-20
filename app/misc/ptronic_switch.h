/**
 * @file       ptronic_switch.h
 * @brief      Pakrtronic activation detection interface.
 *
 * @date       August 18, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_PTRONIC_SWITCH_H__
#define __MDP_PTRONIC_SWITCH_H__

#include <stdbool.h>

#include "stm32f1xx_hal.h"

#define MDP_PTRONIC_EN_GPIO 	GPIOB
#define MDP_PTRONIC_EN_PIN 	GPIO_PIN_14
#define MDP_PTRONIC_EN_LEVEL 	GPIO_PIN_RESET

static inline bool mdp_ptronic_is_enabled(void)
{
	return HAL_GPIO_ReadPin(MDP_PTRONIC_EN_GPIO, MDP_PTRONIC_EN_PIN) ==
		MDP_PTRONIC_EN_LEVEL;
}

#endif /* __MDP_PTRONIC_SWITCH_H__ */
