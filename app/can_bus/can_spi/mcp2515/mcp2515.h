/**
 * @file       mcp2515.h
 * @brief      MCP2515 CAN Controller driver interface implementation.
 *
 * @date       April 9, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_MCP2515_DRIVER_H__
#define __MDP_MCP2515_DRIVER_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/**
 * @brief MCP2515 driver successful return code.
 *        Return codes of other errors are taken from <errno.h>.
 */
#define EOK 0

/**
 * @brief MCP2515 can handle maximum 8-byte messages.
 */
#define CAN_MSG_MAX_SIZE 8

/**
 * @brief Number of RX Buffer 0 acceptance filters.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 25.
 */
#define MCP2515_RXB0_FILTER_NUM 2

/**
 * @brief Number of RX Buffer 1 acceptance filters.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 25.
 */
#define MCP2515_RXB1_FILTER_NUM 4

/**
 * @brief CAN Bus message type enumeration.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 15.
 */
typedef enum {
	MCP2515_MSG_STD_DATA = 0, /* Standart data frame */
	MCP2515_MSG_STD_REMOTE, /* Standart remote frame */
	MCP2515_MSG_EXD_DATA, /* Extended data frame */
	MCP2515_MSG_EXD_REMOTE /* Extended remote frame */
} mcp2515_canid_type_t;

/**
 * @brief MCP2515 TX Buffer enumeration.
 *
 *        NOTE: MCP2515 has 3 transmit buffers.
 *              For more detail information please refer to
 *              MCP2515 datasheet, page 15.
 */
typedef enum {
	MCP2515_TX_BUF_AUTO = 0, /* Select TX buffer automatically */
	MCP2515_TX_BUF0, /* Select TX buffer 0 */
	MCP2515_TX_BUF1, /* Select TX buffer 1 */
	MCP2515_TX_BUF2, /* Select TX buffer 2 */
} mcp2515_tx_buf_t;

/**
 * @brief MCP2515 CAN Bus message structure.
 */
typedef struct {
	uint32_t id; /* CAN Message Identifier */
	mcp2515_canid_type_t id_type; /* CAN Message Identifier type */
	uint8_t size; /* CAN Message payload size */
	uint8_t data[CAN_MSG_MAX_SIZE]; /* CAN Message payload */
} mcp2515_can_msg_t;

/**
 * @brief A structure to store MCP2515 mask registers:
 *        RXMnSIDx and RXMnEIDx.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 35-38.
 */
typedef struct {
	uint8_t sid_h; /* Standart identifier register high */
	uint8_t sid_l; /* Standart identifier register low */
	uint8_t eid_h; /* Extended identifier register high */
	uint8_t eid_l; /* Extended identifier register high */
} mcp2515_rxbm_t;

/**
 * @brief A structure to store MCP2515 filter registers:
 *        RXFnSIDx and RXFnEIDx.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 35-38.
 */
typedef mcp2515_rxbm_t mcp2515_rxbf_t;

/**
 * @brief A receive buffers enumeration,
 *        MCP2515 has 2 receive buffers.
 *
 *        NOTE: For more detail information please refer to
 *              MCP2515 datasheet, page 23.
 */
typedef enum {
	MCP2515_RXB0 = 0,
	MCP2515_RXB1,
	__MCP2515_RXB_COUNT
} mcp2515_rxb_t;

/**
 * @brief Receive buffer filter configuration structure.
 *
 * NOTE:
 * Thuth table (MCP2515 datasheet, page 33, table 4-2):
 *      -------------------------------------------------------
 *      | Mask Bit n | Filter Bit n | Message ID Bit | Action |
 *      -------------------------------------------------------
 *      |      0     |       x      |        x       | Accept |
 *      |      1     |       0      |        0       | Accept |
 *      |      1     |       0      |        1       | Reject |
 *      |      1     |       1      |        0       | Reject |
 *      |      1     |       1      |        1       | Accept |
 *      -------------------------------------------------------
 *      Note: x = don't care
 */
typedef struct mcp2515_rx_filter {
	mcp2515_rxbm_t mask;
	mcp2515_rxbf_t filter[MCP2515_RXB1_FILTER_NUM];
} mcp2515_rx_filter_t;

/**
 * @brief MCP2515 Crystal oscillator frequency enumeration.
 *        Frequency of crystal oscillator connected to OSC1, OSC2
 *        or frequency of External clock input.
 */
typedef enum mcp2515_osc {
	MCP2515_FOSC_8MHZ = 0,
	MCP2515_FOSC_16MHZ,
	__MCP2515_FOSC_COUNT
} mcp2515_osc_t;

/**
 * @brief MCP2515 CAN Bus speed enumeration.
 */
typedef enum mcp2515_speed {
	MCP2515_SPEED_125KBPS = 0,
	MCP2515_SPEED_250KBPS,
	MCP2515_SPEED_500KBPS,
	MCP2515_SPEED_1000KBPS,
	__MCP2515_SPEED_COUNT
} mcp2515_speed_t;

/**
 * @brief  Initialize MCP2515 chip.
 *
 * @param  [in] osc MCP2515 crystal oscillator frequency.
 * @param  [in] speed CAN Bus speed.
 *
 * @return EOK if initialization finished successfully,
 *         error code otherwise.
 */
int mcp2515_init(mcp2515_osc_t osc, mcp2515_speed_t speed);

/**
 * @brief  Configure CAN Bus receive filters.
 *
 * @param  [in] rxb Receive buffer ID to apply filters.
 * @param  [in] filter Filter configuration to apply.
 *
 * @return EOK if filter configuration finished successfully,
 *         error code otherwise or -EBUSY if MCP2515 chip is busy.
 *
 *         NOTE: MCP2515 chip will be busy until
 *               the chip finishes transferring data.
 */
int mcp2515_apply_filters(mcp2515_rxb_t rxb, mcp2515_rx_filter_t *filter);

/**
 * @brief  Check for new received messages in MCP2515 buffers.
 *
 * @return Number of new messages in MCP2515 receive buffers,
 *         error code otherwise.
 */
int mcp2515_messages_available(void);

/**
 * @brief  Check for Bus-Off error.
 *
 * @return 1 if there is no Bus-Off error, 0 if there is Bus-Off error,
 *         error code otherwise.
 */
int mcp2515_check_bus_on(void);

/**
 * @brief  Check for passive tx error.
 *         Actually, the function checks for connection between
 *         MCP2515 and CAN Transceiver (transmitter only).
 *
 * @return 1 if there is passive tx error, 0 if there is no passive tx error,
 *         error code otherwise.
 *
 *         NOTE: For more detail information about errors please refer to
 *               MCP2515 datasheet, page 47.
 */
int mcp2515_check_passive_tx_error(void);

/**
 * @brief  Check for passive rx error.
 *         Actually, the function checks for connection between
 *         MCP2515 and CAN Transceiver (transmitter only).
 *
 * @return 1 if there is passive rx error, 0 if there is no passive rx error,
 *         error code otherwise.
 *
 *         NOTE: For more detail information about errors please refer to
 *               MCP2515 datasheet, page 47.
 */
int mcp2515_check_passive_rx_error(void);

/**
 * @brief  Transmit CAN Bus message.
 *
 * @param  [in] tx_buf MCP2515 transmit buffer ID.
 * @param  [in] tx_msg CAN Bus message.
 *
 * @return EOK if message transmitting finished successfully,
 *         error code otherwise.
 */
int mcp2515_tx_message(mcp2515_tx_buf_t tx_buf, mcp2515_can_msg_t *tx_msg);

/**
 * @brief  Receive CAN Bus message.
 *
 * @param  [out] rx_msg CAN Bus message.
 *
 * @return EOK if message receiving finished successfully,
 *         error code otherwise.
 */
int mcp2515_rx_message(mcp2515_can_msg_t *rx_msg);

/**
 * @brief  Activate MCP2515 sleep mode.
 *
 * @return EOK if MCP2515 sleep mode activation finished successfully,
 *         error code otherwise.
 */
int mcp2515_sleep_mode(void);

/**
 * @brief  Reinitialize the internal registers of the MCP2515 chip.
 *
 * @return EOK if MCP2515 reinitialization finished successfully,
 *         error code otherwise.
 */
int mcp2515_reset(void);

#endif /* __MDP_MCP2515_DRIVER_H__ */
