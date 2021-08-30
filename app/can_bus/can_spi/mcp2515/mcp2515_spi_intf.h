/**
 * @file       mcp2515_spi_intf.h
 * @brief      SPI interface wrapper, the purpose is to use different MCU's
 *             with MCP2515 controller.
 *
 * @date       April 9, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_MCP2515_SPI_INTF_H__
#define __MDP_MCP2515_SPI_INTF_H__

#include <stdbool.h>

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;

#define __MCU_SPI_CS_GPIO_PORT (GPIOA)
#define __MCU_SPI_CS_GPIO_PIN (GPIO_PIN_4)

#define __MCU_SPI_INTF (&hspi1)
#define __MCU_SPI_TIMEOUT (10)
#endif /* STM32F103xB */

static inline bool mcp2515_intf_spi_ready(void)
{
#ifdef STM32F103xB
	return (HAL_SPI_GetState(__MCU_SPI_INTF) == HAL_SPI_STATE_READY);
#endif /* STM32F103xB */
	return false;
}

static inline int mcp2515_intf_spi_transmit(uint8_t *data, uint16_t size)
{
#ifdef STM32F103xB
	return HAL_SPI_Transmit(__MCU_SPI_INTF, data, size, __MCU_SPI_TIMEOUT);
#endif /* STM32F103xB */
	return false;
}

static inline int mcp2515_intf_spi_receive(uint8_t *data, uint16_t size)
{
#ifdef STM32F103xB
	return HAL_SPI_Receive(__MCU_SPI_INTF, data, size, __MCU_SPI_TIMEOUT);
#endif /* STM32F103xB */
	return false;
}

static inline void mcp2515_intf_spi_cs_high(void)
{
#ifdef STM32F103xB
	HAL_GPIO_WritePin(__MCU_SPI_CS_GPIO_PORT, __MCU_SPI_CS_GPIO_PIN,
			  GPIO_PIN_SET);
#endif /* STM32F103xB */
}

static inline void mcp2515_intf_spi_cs_low(void)
{
#ifdef STM32F103xB
	HAL_GPIO_WritePin(__MCU_SPI_CS_GPIO_PORT, __MCU_SPI_CS_GPIO_PIN,
			  GPIO_PIN_RESET);
#endif /* STM32F103xB */
}

#endif /* __MDP_MCP2515_SPI_INTF_H__ */
