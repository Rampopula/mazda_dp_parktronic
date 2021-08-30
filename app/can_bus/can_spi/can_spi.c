#include "can_spi.h"
#include "common.h"
#include "log.h"
#include "mcp2515.h"

#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "can_spi"

int mdp_can_spi_start(void)
{
	/**
	 * We don't need any filtering, so we need to write zeros only
	 * in acceptance mask registers. No need to write acceptance filters,
	 * since the table shows that its value does not affect.
	 */

	int ret = 0;

	/* Acceptance Mask of RXB0 and RXB1 receive buffers */
	mcp2515_rx_filter_t filter = {
		.mask = { 0x0, 0x0, 0x0, 0x0 },
	};

	ret = mcp2515_init(MCP2515_FOSC_8MHZ, MCP2515_SPEED_125KBPS);
	if (ret) {
		log_err("Initialization failed: %s!\r\n", strerror(ret));
		return ret;
	}

	/* Apply filters for RXB0 */
	ret = mcp2515_apply_filters(MCP2515_RXB0, &filter);
	if (ret) {
		log_err("Filter init failed: %s!\r\n", strerror(ret));
		return ret;
	}

	/* Apply filters for RXB1 */
	ret = mcp2515_apply_filters(MCP2515_RXB1, &filter);
	if (ret) {
		log_err("Filter init failed: %s!\r\n", strerror(ret));
		return ret;
	}

	return ret;
}

int mdp_can_spi_stop(void)
{
	return 0;
}

int mdp_can_spi_read(uint32_t *msg_id, uint8_t *data, uint32_t *size)
{
	int ret = 0;
	mcp2515_can_msg_t msg;

	if (!msg_id || !data || !size) {
		log_err("Invalid params: msg_id = %p, data = %p, size = %p\r\n",
			msg_id, data, size);
		return -EINVAL;
	}

	ret = mcp2515_rx_message(&msg);
	if (ret) {
		log_err("CAN read failed: %d\r\n", ret);
		return ret;
	}

	*msg_id = msg.id;
	*size = msg.size;
	memcpy(data, msg.data, msg.size);

	return ret;
}

int mdp_can_spi_write(uint32_t msg_id, uint8_t *data, uint32_t size)
{
	int ret = 0;
	mcp2515_can_msg_t msg;

	if (!data || !size) {
		log_err("Invalid params: data = %p, size = %lu\r\n",
			data, size);
		return -EINVAL;
	}

	msg.id = msg_id;
	msg.id_type = MCP2515_MSG_STD_DATA;
	msg.size = size;
	memcpy(msg.data, data, size);

	ret = mcp2515_tx_message(MCP2515_TX_BUF_AUTO, &msg);
	if (ret)
		log_err("CAN write failed: %d\r\n", ret);

	return ret;
}
