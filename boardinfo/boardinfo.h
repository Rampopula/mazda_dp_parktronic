/**
 * @file       boardinfo.h
 * @brief      Board infromational definitions.
 *
 * @date       August 16, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_BOARDINFO_H__
#define __MDP_BOARDINFO_H__

#include "stm32f1xx_hal.h"

#define MDP_CAN_TEST		0
#define MDP_PTRONIC_TEST	0

#ifndef MDP_APP_DEBUG
#define MDP_APP_DEBUG		1
#endif

#if (MDP_APP_DEBUG == 1)
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

#define MDP_PTRONIC_F2616
#define MDP_BEEPER_ENABLED	1

#define MDP_OVERRIDE_GREETING	1
#define MDP_GREETING_MESSAGE	"  MDP v2.0b "

#define MDP_APP_VERSION		"v1.1"
#define MDP_BOARD_REVISION	"rev.02"
#define MDP_BOARD_CHIP_NAME	"STM32F103C8T6"

#define MDP_CLOCK_FREQ_HZ	HAL_RCC_GetHCLKFreq()
#define MDP_CLOCK_FREQ_MHZ	(MDP_CLOCK_FREQ_HZ / (uint32_t)1e6)

#define MDP_DEBUG_UART_SPEED	115200

#endif /* __MDP_BOARDINFO_H__ */
