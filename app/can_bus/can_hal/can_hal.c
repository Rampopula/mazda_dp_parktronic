#include "can_hal.h"
#include "common.h"
#include "log.h"

#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "can_hal"

#define MDP_HAL_CAN_MIN_MSG_LEN	8
#define MDP_HAL_CAN_MAX_MSG_LEN	8

int mdp_can_hal_start(void)
{
	CAN_FilterTypeDef filter_conf;
	int ret = 0;

	filter_conf.FilterBank = 0;
	filter_conf.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_conf.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_conf.FilterIdHigh = 0x0000;
	filter_conf.FilterIdLow = 0x0000;
	filter_conf.FilterMaskIdHigh = 0x0000;
	filter_conf.FilterMaskIdLow = 0x0000;
	filter_conf.FilterFIFOAssignment = CAN_RX_FIFO0;
	filter_conf.FilterActivation = ENABLE;
	filter_conf.SlaveStartFilterBank = 14;

	ret = HAL_CAN_ConfigFilter(&hcan, &filter_conf);
	if (ret) {
		log_err("Config filter failed: 0x%lx\r\n", hcan.ErrorCode);
		return -EFAULT;
	}

	ret = HAL_CAN_Start(&hcan);
	if (ret) {
		log_err("CAN start failed: 0x%lx\r\n", hcan.ErrorCode);
		return -EFAULT;
	}

	return 0;
}

int mdp_can_hal_stop(void)
{
	int ret = 0;

	ret = HAL_CAN_Stop(&hcan);
	if (ret) {
		log_err("CAN stop failed: 0x%lx\r\n", hcan.ErrorCode);
		ret = -EFAULT;
	}

	return ret;
}

int mdp_can_hal_read(uint32_t *msg_id, uint8_t *data, uint32_t *size)
{
	CAN_RxHeaderTypeDef rx_header;
	int ret = 0;

	if (!data) {
		log_err("Invalid pointer!\r\n");
		return -EFAULT;
	}

	if (!HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0))
		return 0;

	ret = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &rx_header, data);
	if (ret) {
		log_err("CAN read failed: 0x%lx\r\n", hcan.ErrorCode);
		return -EFAULT;
	}

	*msg_id = rx_header.StdId;
	*size = rx_header.DLC;

	return *size;
}

int mdp_can_hal_write(uint32_t msg_id, uint8_t *data, uint32_t size)
{
	CAN_TxHeaderTypeDef tx_header = {
		.ExtId = 0x00,
		.RTR = CAN_RTR_DATA,
		.IDE = CAN_ID_STD,
		.TransmitGlobalTime = DISABLE
	};
	int ret = 0;
	uint32_t tx_mailbox;

	if (!data) {
		log_err("Invalid pointer!\r\n");
		return -EFAULT;
	}

	if (!IN_RANGE(size, MDP_HAL_CAN_MIN_MSG_LEN, MDP_HAL_CAN_MAX_MSG_LEN)) {
		log_err("Invalid message size: %lu!\r\n", size);
		return -EINVAL;
	}

	tx_header.StdId = msg_id;
	tx_header.DLC = size;

	ret = HAL_CAN_AddTxMessage(&hcan, &tx_header, data, &tx_mailbox);
	if (ret) {
		log_err("CAN write failed: 0x%lx\r\n", hcan.ErrorCode);
		return -EFAULT;
	}

	return size;
}
