/**
 * @file       beeper_intf.h
 * @brief      Beeper interface wrapper, the purpose is to use different MCU's
 *             with the beeper API.
 *
 * @date       August 20, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_BEEPER_INTF_H__
#define __MDP_BEEPER_INTF_H__

#include <stdint.h>
#include <stdbool.h>

#include "boardinfo.h"

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;
#endif /* STM32F103xB */

static inline bool mdp_intf_beeper_start(void)
{
#ifdef STM32F103xB
	return HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2) == HAL_OK;
#else
	return false;
#endif /* STM32F103xB */
}

static inline bool mdp_intf_beeper_stop(void)
{
#ifdef STM32F103xB
	return HAL_TIM_OC_Stop(&htim1, TIM_CHANNEL_2) == HAL_OK;
#else
	return false;
#endif /* STM32F103xB */
}

static inline void mdp_intf_beeper_set_freq(uint32_t freq)
{
#ifdef STM32F103xB
	/**
	 * TIM1 APB clock is equal to system HCLK.
	 *
	 * CLK / count_period                       CLK
	 * ------------------ = out_freq   =>   ------------ = count_period
	 *        2                             out_freq * 2
	 */
	uint32_t count_period = MDP_CLOCK_FREQ_HZ / (freq * 2);
	__HAL_TIM_SET_AUTORELOAD(&htim1, count_period);
#endif /* STM32F103xB */
}

#endif /* __MDP_BEEPER_INTF_H__ */
