#include "mdp.h"
#include "boardinfo.h"
#include "common.h"
#include "can_bus.h"
#include <string.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE		"mdp_can_test"

#define NONE			"\033[m"
#define RED				"\033[0;32;31m"
#define GREEN			"\033[0;32;32m"
#define YELLOW			"\033[1;33m"

#if (MDP_CAN_TEST == 1)
void mdp_can_test(void)
{
	int fails = 0;
	struct mdp_can dp_can = mdp_get_can_spi_interface();
	struct mdp_can pjb_can = mdp_get_can_hal_interface();
	const char *line = "*****************************************";

	log_sys(YELLOW"%s\r\n"NONE, line);
	log_sys(YELLOW"PERFOMING MDP CAN BUS TEST\r\n"NONE);

	if (mdp_can_start(&dp_can)) {
		fails++;
		log_sys(RED"SPI CAN [INIT] TEST =========== [FAILED]\r\n"NONE);
	} else {
		log_sys(GREEN"SPI CAN [INIT] TEST =========== [PASSED]\r\n"NONE);
	}

	if (mdp_can_start(&pjb_can)) {
		fails++;
		log_sys(RED"HAL CAN [INIT] TEST =========== [FAILED]\r\n"NONE);
	} else {
		log_sys(GREEN"HAL CAN [INIT] TEST =========== [PASSED]\r\n"NONE);
	}

	{
		dp_can.msg.id = 0x100;
		dp_can.msg.size = 8;
		sprintf((char *)dp_can.msg.data, "testdat");

		if (mdp_can_write(&dp_can)) {
			fails++;
			log_sys(RED"SPI CAN [SEND] TEST =========== [FAILED]\r\n"NONE);
		} else {
			log_sys(GREEN"SPI CAN [SEND] TEST =========== [PASSED]\r\n"NONE);
		}

		if (mdp_can_read(&pjb_can) < 0) {
			fails++;
			log_sys(RED"HAL CAN [RECV] TEST =========== [FAILED]\r\n"NONE);
		} else {
			log_sys(GREEN"HAL CAN [RECV] TEST =========== [PASSED]\r\n"NONE);

			if (pjb_can.msg.id != dp_can.msg.id) {
				fails++;
				log_sys(RED"HAL CAN [MSG_ID] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"HAL CAN [MSG_ID] TEST =========== [PASSED]\r\n"NONE);
			}

			if (pjb_can.msg.size != dp_can.msg.size) {
				fails++;
				log_sys(RED"HAL CAN [MSG_SIZE] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"HAL CAN [MSG_SIZE] TEST =========== [PASSED]\r\n"NONE);
			}

			if (memcmp(pjb_can.msg.data, dp_can.msg.data, sizeof(pjb_can.msg.data))) {
				fails++;
				log_sys(RED"HAL CAN [MSG_DATA] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"HAL CAN [MSG_DATA] TEST =========== [PASSED]\r\n"NONE);
			}
		}
	}
	{
		pjb_can.msg.id = 0x200;
		pjb_can.msg.size = 8;
		sprintf((char *)pjb_can.msg.data, "dattest");

		if (mdp_can_write(&pjb_can) < 0) {
			fails++;
			log_sys(RED"HAL CAN [SEND] TEST =========== [FAILED]\r\n"NONE);
		} else {
			log_sys(GREEN"HAL CAN [SEND] TEST =========== [PASSED]\r\n"NONE);
		}

		if (mdp_can_read(&dp_can) < 0) {
			fails++;
			log_sys(RED"SPI CAN [RECV] TEST =========== [FAILED]\r\n"NONE);
		} else {
			log_sys(GREEN"SPI CAN [RECV] TEST =========== [PASSED]\r\n"NONE);

			if (dp_can.msg.id != pjb_can.msg.id) {
				fails++;
				log_sys(RED"SPI CAN [MSG_ID] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"SPI CAN [MSG_ID] TEST =========== [PASSED]\r\n"NONE);
			}

			if (dp_can.msg.size != pjb_can.msg.size) {
				fails++;
				log_sys(RED"SPI CAN [MSG_SIZE] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"SPI CAN [MSG_SIZE] TEST =========== [PASSED]\r\n"NONE);
			}

			if (memcmp(dp_can.msg.data, pjb_can.msg.data, sizeof(dp_can.msg.data))) {
				fails++;
				log_sys(RED"SPI CAN [MSG_DATA] TEST =========== [FAILED]\r\n"NONE);
			} else {
				log_sys(GREEN"SPI CAN [MSG_DATA] TEST =========== [PASSED]\r\n"NONE);
			}
		}
	}

	if (fails == 0) {
		log_sys(GREEN"MDP CAN TEST [PASSED]\r\n"NONE);
	} else {
		log_sys(RED"MDP CAN TEST [FAILED] %d tests\r\n"NONE, fails);
	}

	log_sys(YELLOW"%s\r\n"NONE, line);
}
#endif
