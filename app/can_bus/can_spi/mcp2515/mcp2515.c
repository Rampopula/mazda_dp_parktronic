/**
 * @file       mcp2515.c
 * @brief      MCP2515 CAN Controller driver implementation.
 *
 * @date       April 9, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#include "mcp2515.h"
#include "mcp2515_internal.h"
#include "mcp2515_spi_intf.h"

#define spi_tx(byte)                                                           \
	({                                                                     \
		uint8_t __byte = byte;                                         \
		mcp2515_intf_spi_transmit(&__byte, sizeof(__byte));            \
	})
#define spi_rx(byte)		mcp2515_intf_spi_receive(byte, sizeof(*byte));
#define spi_rx_buf(buf, len)	mcp2515_intf_spi_receive(buf, len)
#define spi_tx_buf(buf, len)	mcp2515_intf_spi_transmit(buf, len)

#define spi_ready()		mcp2515_intf_spi_ready()
#define spi_cs_high()		mcp2515_intf_spi_cs_high()
#define spi_cs_low()		mcp2515_intf_spi_cs_low()

#define mcp2515_config_on()	mcp2515_set_mode(MCP2515_CONFIG_MODE)
#define mcp2515_config_off()	mcp2515_set_mode(MCP2515_NORMAL_MODE)

/**
 * CAN Bus timings configurations
 *
 * Refer to MCP2515 datasheet page 43 or use online calculator:
 * https://www.kvaser.com/support/calculators/bit-timing-calculator/
 */
static struct mcp2515_cnf_conf {
	uint8_t cnf3, cnf2, cnf1;
} mcp2515_cnf_conf[__MCP2515_FOSC_COUNT][__MCP2515_SPEED_COUNT] = {
	{
		/* Frequency 8 MHz  */
		{ 0x07, 0x9a, 0x01 }, /* 125 kbps */
		{ 0x07, 0x9a, 0x00 }, /* 250 kbps */
		{ 0x03, 0x88, 0x00 }, /* 500 kbps */
		{ 0x00, 0x00, 0x00 } /* 1000  kbps (Not supported for 8 MHz) */
	},
	{
		/* Frequency 16 MHz  */
		{ 0x07, 0x9a, 0x03 }, /* 125 kbps */
		{ 0x07, 0x9a, 0x01 }, /* 250 kbps */
		{ 0x07, 0x9a, 0x07 }, /* 500 kbps */
		{ 0x03, 0x88, 0x00 } /* 1000 kbps */
	}
};

static void mcp2515_canid_to_reg(uint32_t can_id, mcp2515_canid_type_t id_type,
				 mcp2515_buf_regs_t *reg)
{
	/**
	 * We need to handle Extended or Standard Identifier,
	 * no matter what frame type is it: remote or data.
	 *
	 * This function is calling only from the library,
	 * so we have guarantee that <id_type> is valid.
	 */
	if (id_type == MCP2515_MSG_STD_DATA ||
	    id_type == MCP2515_MSG_STD_REMOTE) {
		/**
		 * TXBnEID8 and TXBnEID0 is Extended Identifier bits.
		 * It is not used in messages with Standard Identifier.
		 */
		reg->id_regs.eid_h = reg->id_regs.eid_l = 0;

		/**
		 * TXBnSIDL - Transmit buffer standard identifier register low.
		 * bit 7-5: Standard Identifier bits (SID[2:0]).
		 */
		reg->id_regs.sid_l = 0xFF & (can_id << 5);

		/**
		 * TXBnSIDH - Transmit buffer standard identifier register high.
		 * bit 7-0: Standard Identifier bits (SID[10:3]).
		 */
		reg->id_regs.sid_h = 0xFF & (can_id >> 3);
	} else {
		/**
		 * TXBnEID0 - Transmit buffer extended identifier register low.
		 * bit 7-0: Extended Identifier bits (EID[7:0]).
		 */
		reg->id_regs.eid_l = 0xFF & can_id;

		/**
		 * TXBnEID8 - Transmit buffer extended identifier register high.
		 * bit 7-0: Extended Identifier bits (EID[15:8]).
		 */
		reg->id_regs.eid_h = 0xFF & (can_id >> 8);

		/**
		 * TXBnSIDL - Transmit buffer standard identifier register low.
		 * bit 7-5: Standard Identifier bits (SID[2:0]).
		 * bit   3: Extended Identifier Enable bit (EXIDE).
		 * bit 1-0: Extended Identifier bits (EID[17:16]).
		 */
		reg->id_regs.sid_l = 0x03 & (can_id >> 16);
		reg->id_regs.sid_l |= (1 << 3);
		reg->id_regs.sid_l |= 0xE0 & (can_id >> 13);

		/**
		 * TXBnSIDH - Transmit buffer standard identifier register high.
		 * bit 7-0: Standard Identifier bits (SID[10:3]).
		 */
		reg->id_regs.sid_h = 0xFF & (can_id >> 21);
	}
}

static uint32_t mcp2515_reg_to_canid(mcp2515_canid_type_t id_type,
				     mcp2515_buf_regs_t *reg)
{
	uint32_t can_id;

	/**
	 * We need to handle Extended or Standard Identifier,
	 * no matter what frame type is it: remote or data.
	 *
	 * This function is calling only from the library,
	 * so we have guarantee that <id_type> is valid.
	 */
	if (id_type == MCP2515_MSG_STD_DATA ||
	    id_type == MCP2515_MSG_STD_REMOTE) {
		/**
		 * TXBnSIDH - Transmit buffer standard identifier register high.
		 * bit 7-0: Standard Identifier bits (SID[10:3]).
		 */
		can_id = (reg->id_regs.sid_h << 3);

		/**
		 * TXBnSIDL - Transmit buffer standard identifier register low.
		 * bit 7-5: Standard Identifier bits (SID[2:0]).
		 */
		can_id |= (reg->id_regs.sid_l >> 5);
	} else {
		/* Parse SIDL register into fields */
		union {
			struct {
				uint8_t eid : 2;
				uint8_t unused : 3;
				uint8_t sid : 3;
			};
			uint8_t data;
		} __sid_l;
		__sid_l.data = reg->id_regs.sid_l;

		/* Order all registers and bits into 29-bit value */
		can_id = reg->id_regs.sid_h << 21; /* bit 28-21: SID[10:3] */
		can_id |= __sid_l.sid << 18; /* bit 20-18: SID[2:0] */
		can_id |= __sid_l.eid << 16; /* bit 17-16: EID[17:16] */
		can_id |= reg->id_regs.eid_h << 8; /* bit 15-8: EID[15:8] */
		can_id |= reg->id_regs.eid_l << 0; /* bit 7-0: EID[7:0] */
	}

	return can_id;
}

static int mcp2515_modify_bit(uint8_t addr, uint8_t mask, uint8_t data)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_BIT_MOD);
	ret |= spi_tx(addr);
	ret |= spi_tx(mask);
	ret |= spi_tx(data);

	spi_cs_high();

	return ret;
}

static int mcp2515_read_status(mcp2515_status_t *status)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_READ_STATUS);
	ret |= spi_rx(&status->data);

	spi_cs_high();

	return ret;
}

static int mcp2515_read_rx_status(mcp2515_rx_status_t *rx_status)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_RX_STATUS);
	ret |= spi_rx(&rx_status->data);

	spi_cs_high();

	return ret;
}

static int mcp2515_request_send(uint8_t instruction)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(instruction);

	spi_cs_high();

	return ret;
}

static int mcp2515_load_tx_seq(uint8_t instruction, mcp2515_buf_regs_t *regs)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(instruction);
	ret |= spi_tx_buf((uint8_t *)&regs->id_regs, sizeof(regs->id_regs));
	ret |= spi_tx(regs->dlc);
	ret |= spi_tx_buf((uint8_t *)&regs->buf, sizeof(regs->buf));

	spi_cs_high();

	return ret;
}

static int mcp2515_read_byte(uint8_t addr, uint8_t *byte)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_READ);
	ret |= spi_tx(addr);
	ret |= spi_rx(byte);

	spi_cs_high();

	return ret;
}

static int mcp2515_read_byte_seq(uint8_t instruction, uint8_t *data,
				 uint8_t length)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(instruction);
	ret |= spi_rx_buf(data, length);

	spi_cs_high();

	return ret;
}

static int mcp2515_write_byte(uint8_t addr, uint8_t data)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_WRITE);
	ret |= spi_tx(addr);
	ret |= spi_tx(data);

	spi_cs_high();

	return ret;
}

static int mcp2515_write_byte_seq(uint8_t start_addr, uint8_t end_addr,
				  uint8_t *data)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_WRITE);
	ret |= spi_tx(start_addr);
	ret |= spi_rx_buf(data, end_addr - start_addr + 1);

	spi_cs_high();

	return ret;
}

static int mcp2515_set_mode(mcp2515_mode_t mode)
{
	int ret = EOK;
	uint8_t byte = 0;
	uint32_t tries_cnt = MCP2515_SET_OPMODE_TRIES;

	if (mode != MCP2515_CONFIG_MODE && mode != MCP2515_NORMAL_MODE &&
	    mode != MCP2515_SLEEP_MODE)
		return -EINVAL;

	ret = mcp2515_write_byte(MCP2515_CANCTRL, (uint8_t)mode);
	if (ret != EOK)
		return ret;

	do {
		ret = mcp2515_read_byte(MCP2515_CANSTAT, &byte);
		if (ret != EOK)
			return ret;

		if ((byte & MCP2515_OPMODE_MASK) == (uint8_t)mode)
			return EOK;
	} while (tries_cnt--);

	return -EAGAIN;
}

int mcp2515_init(mcp2515_osc_t osc, mcp2515_speed_t speed)
{
	int ret = EOK;

	if ((uint8_t)osc >= __MCP2515_FOSC_COUNT ||
	    (uint8_t)speed >= __MCP2515_SPEED_COUNT)
		return -EINVAL;

	if (osc == MCP2515_FOSC_8MHZ && speed == MCP2515_SPEED_1000KBPS)
		return -ENOTSUP;

	spi_cs_high();

	if (!spi_ready())
		return -EBUSY;

	ret = mcp2515_config_on();
	if (ret != EOK)
		return ret;

	ret = mcp2515_write_byte_seq(MCP2515_CNF3, MCP2515_CNF1,
				     (uint8_t *)&mcp2515_cnf_conf[osc][speed]);
	if (ret != EOK)
		return ret;

	ret = mcp2515_config_off();
	if (ret != EOK)
		return ret;

	return EOK;
}

int mcp2515_apply_filters(mcp2515_rxb_t rxb, mcp2515_rx_filter_t *filter)
{
	int ret = EOK;

	if ((uint8_t)rxb >= __MCP2515_RXB_COUNT)
		return -EINVAL;

	ret = mcp2515_config_on();
	if (ret != EOK)
		return ret;

	if (rxb == MCP2515_RXB0) {
		ret |= mcp2515_write_byte_seq(MCP2515_RXM0SIDH,
					      MCP2515_RXM0EID0,
					      (uint8_t *)&filter->mask);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF0SIDH,
					      MCP2515_RXF0EID0,
					      (uint8_t *)&filter->filter[0]);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF1SIDH,
					      MCP2515_RXF1EID0,
					      (uint8_t *)&filter->filter[1]);
	} else {
		ret |= mcp2515_write_byte_seq(MCP2515_RXM1SIDH,
					      MCP2515_RXM1EID0,
					      (uint8_t *)&filter->mask);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF2SIDH,
					      MCP2515_RXF2EID0,
					      (uint8_t *)&filter->filter[0]);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF3SIDH,
					      MCP2515_RXF3EID0,
					      (uint8_t *)&filter->filter[1]);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF4SIDH,
					      MCP2515_RXF4EID0,
					      (uint8_t *)&filter->filter[2]);
		ret |= mcp2515_write_byte_seq(MCP2515_RXF5SIDH,
					      MCP2515_RXF5EID0,
					      (uint8_t *)&filter->filter[3]);
	}

	if (ret != EOK)
		return ret;

	ret = mcp2515_config_off();
	if (ret != EOK)
		return ret;

	return EOK;
}

int mcp2515_messages_available(void)
{
	int ret = EOK;
	mcp2515_status_t status = {
		.data = 0
	};

	ret = mcp2515_read_status(&status);
	if (ret != EOK)
		return ret;

	return status.rx0_int + status.rx1_int;
}

int mcp2515_check_bus_on(void)
{
	int ret = EOK;
	uint8_t byte = 0;

	ret = mcp2515_read_byte(MCP2515_EFLG, &byte);
	if (ret != EOK)
		return ret;

	return !(byte & MCP2515_ERR_TXBO);
}

int mcp2515_check_passive_rx_error(void)
{
	int ret = EOK;
	uint8_t byte = 0;

	ret = mcp2515_read_byte(MCP2515_EFLG, &byte);
	if (ret != EOK)
		return ret;

	return byte & MCP2515_ERR_RXEP;
}

int mcp2515_check_passive_tx_error(void)
{
	int ret = EOK;
	uint8_t byte = 0;

	ret = mcp2515_read_byte(MCP2515_EFLG, &byte);
	if (ret != EOK)
		return ret;

	return byte & MCP2515_ERR_TXEP;
}

int mcp2515_tx_message(mcp2515_tx_buf_t tx_buf, mcp2515_can_msg_t *tx_msg)
{
	int ret = EOK;
	mcp2515_status_t tx_status;
	mcp2515_buf_regs_t tx_regs;

	ret = mcp2515_read_status(&tx_status);
	if (ret != EOK)
		return ret;

	mcp2515_canid_to_reg(tx_msg->id, tx_msg->id_type, &tx_regs);

	/* Trim message size if user don't read docs */
	if (tx_msg->size > 8)
		tx_msg->size = 8;

	/* Copy from user (we can avoid it by rewriting load_rx_seq) */
	tx_regs.dlc = tx_msg->size;
	memcpy(tx_regs.buf, tx_msg->data, tx_regs.dlc);

	/**
	 * Handle Remote Transmission Request (RTR).
	 *
	 * To send remote frame we need to set RTR bit in TXBnDLC register.
	 * 	bit 6: Remote Transmission Request bit (RTR).
	 */
	if (tx_msg->id_type == MCP2515_MSG_STD_REMOTE ||
	    tx_msg->id_type == MCP2515_MSG_EXD_REMOTE) {
		tx_regs.dlc |= 1 << 6;
	}

	switch (tx_buf) {
	default:
		/* Invalid TX buffer, treat as auto */
		tx_buf = MCP2515_TX_BUF_AUTO;
	case MCP2515_TX_BUF_AUTO:
	case MCP2515_TX_BUF0:
		if (!tx_status.tx0_int) {
			ret |= mcp2515_load_tx_seq(MCP2515_LOAD_TXB0SIDH,
						   &tx_regs);
			ret |= mcp2515_request_send(MCP2515_RTS_TX0);

			break;
		}

		if (tx_buf != MCP2515_TX_BUF_AUTO)
			break;

	case MCP2515_TX_BUF1:
		if (!tx_status.tx1_int) {
			ret |= mcp2515_load_tx_seq(MCP2515_LOAD_TXB1SIDH,
						   &tx_regs);
			ret |= mcp2515_request_send(MCP2515_RTS_TX1);

			break;
		}

		if (tx_buf != MCP2515_TX_BUF_AUTO)
			break;

	case MCP2515_TX_BUF2:
		if (!tx_status.tx1_int) {
			ret |= mcp2515_load_tx_seq(MCP2515_LOAD_TXB2SIDH,
						   &tx_regs);
			ret |= mcp2515_request_send(MCP2515_RTS_TX2);

			break;
		}

		if (tx_buf != MCP2515_TX_BUF_AUTO)
			break;
	}

	return ret;
}

int mcp2515_rx_message(mcp2515_can_msg_t *rx_msg)
{
	int ret = EOK;
	mcp2515_rx_status_t rx_status = {
		.data = 0
	};
	mcp2515_buf_regs_t rx_regs;

	ret = mcp2515_read_rx_status(&rx_status);
	if (ret != EOK)
		return ret;

	/* If there is no messages in RX buffers */
	if (!rx_status.rx_buffer)
		return -ENODATA;

	if (rx_status.rx_buffer == MCP2515_MSG_RXB0 ||
	    rx_status.rx_buffer == MCP2515_MSG_BOTH) {
		/* Read message from RXB0 buffer */
		ret = mcp2515_read_byte_seq(MCP2515_READ_RXB0SIDH, rx_regs.data,
					    sizeof(rx_regs.data));
	} else {
		/* Read message from RXB1 buffer */
		ret = mcp2515_read_byte_seq(MCP2515_READ_RXB1SIDH, rx_regs.data,
					    sizeof(rx_regs.data));
	}

	if (ret != EOK)
		return ret;

	rx_msg->id_type = rx_status.msg_type;
	rx_msg->id = mcp2515_reg_to_canid(rx_msg->id_type, &rx_regs);
	rx_msg->size = rx_regs.dlc;
	memcpy(rx_msg->data, rx_regs.buf, rx_msg->size);

	return EOK;
}

int mcp2515_sleep_mode(void)
{
	int ret = EOK;

	ret |= mcp2515_modify_bit(MCP2515_CANINTF, MCP2515_WAKIF_MASK,
				  MCP2515_WAKIF_RESET);
	ret |= mcp2515_modify_bit(MCP2515_CANINTE, MCP2515_WAKIF_MASK,
				  MCP2515_WAKIF_SET);
	if (ret != EOK)
		return ret;

	return mcp2515_set_mode(MCP2515_SLEEP_MODE);
}

int mcp2515_reset(void)
{
	int ret = EOK;

	spi_cs_low();

	ret |= spi_tx(MCP2515_RESET);

	spi_cs_high();

	return ret;
}
