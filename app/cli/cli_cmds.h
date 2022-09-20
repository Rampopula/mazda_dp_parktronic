/**
 * @file       cli_cmds.h
 * @brief      Simple command line interface commands definitions.
 *
 * @date       Septemer 20, 2022
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __CLI_CMDS_H__
#define __CLI_CMDS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "hexdump.h"
#include "can_bypass_switch.h"

static void cmd_help(void *arg)
{
	(void)arg;

	printf("\r\n");
	printf("hexdump [text]\tshow hexdump of [text]\r\n");
	printf("candump\t\trun can bus dumping app\r\n");
	printf("mdp_run\t\trun mazda display parktronic app\r\n");
	printf("app_info\tprint application info\r\n");
	printf("send [text]\tsend [text] to display\r\n");
	printf("send_loop [text]\tsend loopped [text] to display\r\n");
	printf("bypass [on|off]\tget or set CAN bypass switch state\r\n");
}

static void cmd_hexdump(void *arg)
{
	char *param = (char *)arg;
	char *tmp = (char *)param;
	size_t len = 0;

	if (!param) {
		printf("\r\nInvalid hexdump argument!\r\n");
		return;
	}

	while(*tmp != ' ' && *tmp != '\0') {
		tmp++;
		len++;
	}

	hexdump(param, 0, (uint8_t *)param, len);
}

static void cmd_app_info(void *arg)
{
	extern void log_app_info(void);

	(void)arg;

	log_app_info();
}

static void cmd_bypass(void *arg)
{
	if (!arg || !strlen((char *)arg)) {
		printf("\r\n%s\r\n", mdp_can_bypass_get() ? "ON" : "OFF");
		return;
	}

	if (!strcmp((const char *)arg, "on")) {
		mdp_can_bypass_on();
	} else if (!strcmp((const char *)arg, "off")) {
		mdp_can_bypass_off();
	} else {
		printf("\r\n'%s': invalid parameter\r\n",
		       (const char *)arg);
		return;
	}

	printf("\r\nOK\r\n");
}

static void cmd_send(void *arg)
{
	int ret;
	extern int mdp_can_send_text(const char *text);

	ret = mdp_can_send_text((const char *)arg);
	if (ret) {
		printf("\r\nERR: %d\r\n", ret);
	} else {
		printf("\r\nOK\r\n");
	}
}

static void cmd_send_loop(void *arg)
{
	extern void mdp_cmd_send_loop(const char *str);

	mdp_cmd_send_loop((const char *)arg);

	printf("\r\nOK\r\n");
}

static int parse_int(void *arg)
{
	char *num = (char *)arg;
	char *end = NULL;
	unsigned long result = 0;

	result = strtoul(num, &end, 0);
	if (end != num + strlen(num)) {
		printf("\r\nERR: %d\r\n", -EINVAL);
		return -EINVAL;
	}

	if (result == UINT32_MAX && errno == ERANGE) {
		printf("\r\nERR: %d\r\n", -ERANGE);
		return -ERANGE;
	}

	if (result > UINT8_MAX) {
		printf("\r\nERR: %d (MAX 255)\r\n", -ERANGE);
		return -ERANGE;
	}

	return (int)result;
}

static void cmd_set_misc0(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *     CDIN   ST  RPT  AF
	 *	  |   |    |   |
	 *      1 0 0 0  0 0 0 0		0xFF
	 *	    |	 |   |
	 *	  MDIN	DLB RDM
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(0, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc1(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *       TA AUTO-M ?   2
	 *	  |   |    |   |
	 *      0 0 0 0  0 0 0 0		0xF3
	 *	|   |	 |   |
	 *	PTY TP	 ?   1
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(1, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc2(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *        4   6    ?   ?
	 *	  |   |    |   |
	 *      0 0 0 0  0 0 0 0		0xF0
	 *	|   |	 |   |
	 *	3   5	 ?   ?
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(2, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc3(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *	      quote after 10th symb
	 *            |    dot after 11th symb
	 *        ?   |    |   turn off all symb excl 12b ASCII field
	 *	  |   |    |   |
	 *      0 0 0 0  0 0 0 0		0x1E
	 *	|   |	 |   |
	 *	?   |	 |  dot after 10th symb
	 *          |     |
	 *          |     quote after 12th symb
	 *	    |":" symb after 3rd symb
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(3, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc4(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *     CDIN   ST  RPT  AF
	 *	  |   |    |   |
	 *      0 0 0 0  0 0 0 0
	 *	    |	   |   |
	 *	  MDIN	DLB RDM
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(4, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc5(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *     CDIN   ST  RPT  AF
	 *	  |   |    |   |
	 *      1 0 0 0  0 0 0 0
	 *	    |	   |   |
	 *	  MDIN	DLB RDM
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(5, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc6(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *     CDIN   ST  RPT  AF
	 *	  |   |    |   |
	 *      1 0 0 0  0 0 0 0
	 *	    |	   |   |
	 *	  MDIN	DLB RDM
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(6, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_set_misc7(void *arg)
{
	int ret;
	extern void mdp_cmd_set_misc(uint8_t byte, uint8_t data);

	/*
	 *     CDIN   ST  RPT  3
	 *	  |   |    |   |
	 *      0 0 0 0  0 0 0 0
	 *	    |	 |   |
	 *	  MDIN	DLB RDM
	*/

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_cmd_set_misc(7, (uint8_t)ret);

	printf("\r\nOK\r\n");
}

static void cmd_dp_show_num(void *arg)
{
	int ret;
	extern void mdp_dp_show_num(uint8_t num);

	ret = parse_int(arg);
	if (ret < 0)
		return;

	mdp_dp_show_num((uint8_t)ret);

	printf("\r\nOK\r\n");
}

static struct cli_cmds {
	const char *name;
	void (*func)(void *);
} cli_cmds[] = {
	{.name = "help",	.func = cmd_help},
	{.name = "hexdump",	.func = cmd_hexdump},
	{.name = "app_info",	.func = cmd_app_info},
	{.name = "bypass",	.func = cmd_bypass},
	{.name = "send",	.func = cmd_send},
	{.name = "send_loop",	.func = cmd_send_loop},
	{.name = "set_misc0",	.func = cmd_set_misc0},
	{.name = "set_misc1",	.func = cmd_set_misc1},
	{.name = "set_misc2",	.func = cmd_set_misc2},
	{.name = "set_misc3",	.func = cmd_set_misc3},
	{.name = "set_misc4",	.func = cmd_set_misc4},
	{.name = "set_misc5",	.func = cmd_set_misc5},
	{.name = "set_misc6",	.func = cmd_set_misc6},
	{.name = "set_misc7",	.func = cmd_set_misc7},
	{.name = "dp_show_num", .func = cmd_dp_show_num},
};

#endif /* __CLI_CMDS_H__ */
