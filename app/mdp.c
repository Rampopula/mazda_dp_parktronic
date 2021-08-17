#include "mdp.h"

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "mdp_app"

static void log_app_info(void)
{
	const char *line = "*****************************************";
	const char *app_name = "Mazda Display Parktronic";
	const char *author = "by Eduard Chaika <rampopula@gmail.com>";

	printf("\r\n\r\n");
	log_sys("%s\r\n", line);
	log_sys("%s\r\n", app_name);
	log_sys("%s\r\n", author);
	log_sys("%s\r\n", line);
	log_sys("Chip: %s\r\n", MDP_BOARD_CHIP_NAME);
	log_sys("Software version: %s %s\r\n", MDP_APP_VERSION,
		MDP_APP_DEBUG ? "debug" : "release");
	log_sys("Board revision: %s\r\n", MDP_BOARD_REVISION);
	log_sys("Build date: %s\r\n", __TIMESTAMP__);
	log_sys("%s\r\n", line);
}

void mdp_init(void)
{
	log_app_info();
}

void mdp_run(void)
{
}
