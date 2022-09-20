#include "cli.h"
#include "cli_cmds.h"
#include "common.h"
#include "log.h"

#include <string.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE		"cli"

#define CLI_PROMPT		"\r\n> "
#define CLI_CLEAR		"\033c"
#define CLI_TAB			"    "
#define CLI_CMD_NOT_FOUND	"\r\n'%s': command not found\r\n"


#define CLI_ENDSTR_SYMB		'\0'
#define CLI_RETURN_SYMB		'\r'
#define CLI_TAB_SYMB		'\t'
#define CLI_BACKSPACE_SYMB	'\177'

#define CMD_MATCH		1
#define CMD_MATCH_PARAM		2

#define UART_TX_TIMEOUT		5
#define UART_RX_TIMEOUT		5

#define __unwrap(x)		#x
#define __len(x)		__unwrap(x)
#define __cli_cmd(name)  	{ #name, name ## _command }

extern UART_HandleTypeDef huart3;
static UART_HandleTypeDef *serial = &huart3;

static bool cli_initial = true;
static size_t cli_buf_len;
static char cli_buf[CLI_INPUT_BUFFER_SIZE];
static char cli_cmd_param[CLI_CMD_PARAM_LEN];

static inline void cli_flush_buffer(void)
{
	cli_buf[cli_buf_len = 0] = CLI_ENDSTR_SYMB;
}

static inline void cli_prompt(void)
{
	char prompt[] = CLI_PROMPT;

	HAL_UART_Transmit(serial, (uint8_t *)prompt, sizeof(prompt),
			  UART_TX_TIMEOUT);
}

void cli_print_stationary(const char *str)
{
	char clr[] = CLI_CLEAR;

	HAL_UART_Transmit(serial, (uint8_t *)clr, sizeof(clr), UART_TX_TIMEOUT);
	HAL_UART_Transmit(serial, (uint8_t *)str, strlen(str), UART_TX_TIMEOUT);
}

static bool match_command(char *str, const char *cmd)
{
	char _cmd[CLI_CMD_LEN];

	/* Erase previous param */
	*cli_cmd_param = CLI_ENDSTR_SYMB;

	if (sscanf(str, "%"__len(CLI_CMD_LEN)"s %"__len(CLI_CMD_PARAM_LEN)"s",
		   _cmd, cli_cmd_param) != CMD_MATCH_PARAM) {
		if (sscanf(str, "%15s", _cmd) != CMD_MATCH)
			return false;
	}

	return !strcmp(_cmd, cmd);
}

static void cli_process_input(void)
{
	for (int i = 0; i < ARRAY_SIZE(cli_cmds); i++) {
		if (match_command(cli_buf, cli_cmds[i].name)) {
			cli_cmds[i].func(cli_cmd_param);
			goto end;
		}
	}

	printf(CLI_CMD_NOT_FOUND, cli_buf);

end:
	cli_flush_buffer();
}

void cli_process(void)
{
	char ch;
	char tab[] = CLI_TAB;

	if (cli_initial) {
		cli_prompt();
		cli_initial = false;
	}

	if (HAL_UART_Receive(serial, (uint8_t *)&ch, sizeof(ch),
			     UART_RX_TIMEOUT) != HAL_OK)
		return;

	if (ch == CLI_RETURN_SYMB) {
		if (cli_buf[0] != CLI_ENDSTR_SYMB)
			cli_process_input();

		cli_prompt();
		return;
	}

	if (ch == CLI_BACKSPACE_SYMB) {
		if (cli_buf_len) {
			cli_buf[--cli_buf_len] = CLI_ENDSTR_SYMB;
			HAL_UART_Transmit(serial, (uint8_t *)&ch, sizeof(ch),
					  UART_TX_TIMEOUT);
		}

		return;
	}

	if (ch == CLI_TAB_SYMB) {
		if (cli_buf_len >= sizeof(cli_buf) - sizeof(tab))
			return;

		memcpy(&cli_buf[cli_buf_len], tab, sizeof(tab));
		cli_buf_len += sizeof(tab) - 1;
		HAL_UART_Transmit(serial, (uint8_t *)tab, sizeof(tab),
				  UART_TX_TIMEOUT);
	}

	if (cli_buf_len >= sizeof(cli_buf) - 1)
		return;

	cli_buf[cli_buf_len++] = ch;
	cli_buf[cli_buf_len] = CLI_ENDSTR_SYMB;
	HAL_UART_Transmit(serial, (uint8_t *)&ch, sizeof(ch), UART_TX_TIMEOUT);
}
