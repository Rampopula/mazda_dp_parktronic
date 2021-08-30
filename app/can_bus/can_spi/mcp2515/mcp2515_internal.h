/**
 * @file       mcp2515_internal.h
 * @brief      MCP2515 driver internal definitions and data structures.
 *
 * @date       April 9, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_MCP2515_INTERNAL_H__
#define __MDP_MCP2515_INTERNAL_H__

#include "mcp2515.h"
#include "mcp2515_regs.h"

#include <stdint.h>

/**
 * When changing modes, the mode will not actually change until all
 * pending message transmissions are complete. The requested mode must
 * be verified byreading the OPMODE[2:0] bits (CANSTAT[7:5]).
 */
#define MCP2515_OPMODE_MASK (0xE0) /* CANSTAT[7:5] */
#define MCP2515_SET_OPMODE_TRIES (20000) /* Wait timeout for mode change */
#define MCP2515_MAX_MSG_SIZE 8 /* MCP2515 can handle maximum 8-byte messages */

/* Interrupts */
#define MCP2515_WAKIF_MASK (1 << 6) /* Wakeup interrupt flag mask */
#define MCP2515_WAKIF_SET (1 << 6) /* Wakeup interrupt flag set */
#define MCP2515_WAKIF_RESET (0x00) /* Wakeup interrupt flag reset */

/**
 * @brief MCP2515 Error flags enumeration.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 47.
 *
 *        NOTE: MCP2515 driver has an interface that only
 *              checks TXEP, RXEP and TXBO flags, other things
 *              currently are not implemented.
 */
typedef enum {
	MCP2515_ERR_EWARN = (1 << 0), /* Error Warning Flag bit */
	MCP2515_ERR_RXWAR = (1 << 1), /* Receive Error Warning Flag bit */
	MCP2515_ERR_TXWAR = (1 << 2), /* Transmit Error Warning Flag bit */
	MCP2515_ERR_RXEP = (1 << 3), /* Receive Error-Passive Flag bit */
	MCP2515_ERR_TXEP = (1 << 4), /* Transmit Error-Passive Flag bit */
	MCP2515_ERR_TXBO = (1 << 5), /* Bus-Off Error Flag bit */
	MCP2515_ERR_RX0OVR = (1 << 6), /* Receive Buffer 0 Overflow Flag bit */
	MCP2515_ERR_RX1OVR = (1 << 7), /* Receive Buffer 1 Overflow Flag bit */
} mcp2515_error_flag_t;

/**
 * @brief A union to map MCP2515 CAN interrupt flags and get chip status.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 54.
 */
typedef union {
	struct {
		uint8_t rx0_int : 1; /* RX Buffer 0 Full Interrupt Flag bit */
		uint8_t rx1_int : 1; /* RX Buffer 1 Full Interrupt Flag bit */
		uint8_t tx0_int : 1; /* TX Buffer 0 Empty Interrupt Flag bit */
		uint8_t tx1_int : 1; /* TX Buffer 1 Empty Interrupt Flag bit */
		uint8_t tx2_int : 1; /* TX Buffer 2 Empty Interrupt Flag bit */
		uint8_t err_int : 1; /*  Error Interrupt Flag bit */
		uint8_t wake_int : 1; /* Wake-up Interrupt Flag bit */
		uint8_t msg_err_int : 1; /* Message Error Interrupt Flag bit */
	};
	uint8_t data;
} mcp2515_status_t;

/**
 * @brief A union to map MCP2515 RX Status flags.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 70.
 */
typedef union {
	struct {
		uint8_t filter_match : 3; /* RX message filter match type */
		uint8_t msg_type : 2; /* Type of received message */
		uint8_t unused : 1; /* Unused bit */
		uint8_t rx_buffer : 2; /* ID of buffer with received message */
	};
	uint8_t data;
} mcp2515_rx_status_t;

/**
 * @brief A structure to store MCP2515 identifier registers:
 *        RXBnSIDH, RXBnSIDL, RXBnEIDH, RXBnEIDL,
 *        TXBnSIDH, TXBnSIDL, TXBnEIDH, TXBnEIDL.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 30-31.
 */
typedef struct {
	uint8_t sid_h; /* Standart identifier register high */
	uint8_t sid_l; /* Standart identifier register low */
	uint8_t eid_h; /* Extended identifier register high */
	uint8_t eid_l; /* Extended identifier register high */
} mcp2515_regs_t;

/**
 * @brief A structure to store MCP2515 TX and RX registers and data buffer.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 20-22.
 */
typedef union {
	struct {
		mcp2515_regs_t id_regs; /* MCP2515 identifier registers */
		uint8_t dlc; /* TX or RX Data length code (buffer size) */
		uint8_t buf[MCP2515_MAX_MSG_SIZE]; /* TX or RX buffer */
	};
	uint8_t data[MCP2515_MAX_MSG_SIZE + 5]; /* size(id_regs) + size(dlc) */
} mcp2515_buf_regs_t;

/**
 * @brief MCP2515 Modes of operation enumeration.
 *
 *        NOTE: Loopback and Listen-Only modes currently
 *              are not implemented.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 59.
 */
typedef enum {
	MCP2515_NORMAL_MODE = 0x00,
	MCP2515_SLEEP_MODE = 0x20,
	MCP2515_CONFIG_MODE = 0x80
} mcp2515_mode_t;

#endif /* __MDP_MCP2515_INTERNAL_H__ */
