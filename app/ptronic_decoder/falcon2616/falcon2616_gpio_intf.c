/**
 * @file       falcon2616_gpio_intf.c
 * @brief      GPIO interface wrapper implementation.
 *
 * @date       April 29, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#include "falcon2616_gpio_intf.h"

inline uint8_t f2616_intf_gpio_read(void)
{
	uint8_t data;
#ifdef STM32F103xB
	data = HAL_GPIO_ReadPin(__MCU_PTRONIC_DATA_GPIO_PORT,
				__MCU_PTRONIC_DATA_GPIO_PIN);
#endif /* STM32F103xB */
	return data;
}

inline void __GPIO_DATA_READ_IRQ(void)
{
	if (__F2616_GPIO_IRQ_CB)
		__F2616_GPIO_IRQ_CB();
}
