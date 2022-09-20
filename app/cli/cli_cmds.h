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
#include "hexdump.h"

static void cmd_help(void *arg)
{
	(void)arg;

	printf("\r\n");
	printf("hexdump [text]\tshow hexdump of [text]\r\n");
	printf("candump\t\trun can bus dumping app\r\n");
	printf("mdp_run\t\trun mazda display parktronic app\r\n");
	printf("app_info\tprint application info\r\n");
	printf("app_status\tprint application status\r\n");
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

static void cmd_app_status(void *arg)
{
	extern const char *mdp_get_status(void);

	(void)arg;

	printf("\r\n%s\r\n", mdp_get_status());
}

static struct cli_cmds {
	const char *name;
	void (*func)(void *);
} cli_cmds[] = {
	{.name = "help",	.func = cmd_help},
	{.name = "hexdump",	.func = cmd_hexdump},
	{.name = "app_info",	.func = cmd_app_info},
	{.name = "app_status",	.func = cmd_app_status},
};

#endif /* __CLI_CMDS_H__ */
