/**
 * @file       cli.h
 * @brief      Simple command line interface implemntation interface.
 *
 * @date       Septemer 20, 2022
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __CLI_H__
#define __CLI_H__

#include "boardinfo.h"

#define CLI_INPUT_BUFFER_SIZE	128 /* Maximum console input length */

#define CLI_CMD_LEN		16  /* CLI command max length */
#define CLI_CMD_PARAM_LEN	16  /* CLI command arguments max length */

void cli_process(void);

void cli_print_stationary(const char *str);

#endif /* __CLI_H__ */
