/**
 * @file       falcon2616_gpio_intf.h
 * @brief      GPIO interface wrapper, the purpose is to use different MCU's
 *             with the Falcon 2616 Parktronic decoder.
 *
 * @date       April 29, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_F2616_GPIO_INTF_H__
#define __MDP_F2616_GPIO_INTF_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"

#define __MCU_PTRONIC_DATA_GPIO_PORT	GPIOB
#define __MCU_PTRONIC_DATA_GPIO_PIN	GPIO_PIN_12
#endif /* STM32F103xB */

/**
 * @brief Used-end GPIO IRQ callback function.
 *
 *         NOTE: This function is called from
 *               __GPIO_DATA_READ_IRQ function.
 */
extern void (*__F2616_GPIO_IRQ_CB)(void);

uint8_t f2616_intf_gpio_read(void);

/**
 * @brief GPIO IRQ callback function.
 *
 *         NOTE: This function must be called
 *               inside the HAL Library for the
 *               __MCU_PTRONIC_DATA_GPIO_PIN.
 */
void __GPIO_DATA_READ_IRQ(void);

#endif /* __MDP_F2616_GPIO_INTF_H__ */
