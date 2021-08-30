/**
 * @file       can_spi.h
 * @brief      CAN SPI interface wrapper.
 *
 * @date       August 25, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_CAN_SPI_H__
#define __MDP_CAN_SPI_H__

#include <stdint.h>

int mdp_can_spi_start(void);
int mdp_can_spi_stop(void);
int mdp_can_spi_read(uint32_t *msg_id, uint8_t *data, uint32_t *size);
int mdp_can_spi_write(uint32_t msg_id, uint8_t *data, uint32_t size);

#endif /* __MDP_CAN_SPI_H__*/
