#include "mdp.h"
#include "beeper.h"
#include "common.h"
#include "can_bus.h"
#include "can_bypass_switch.h"
#include "ptronic_decoder.h"
#include "ptronic_switch.h"
#include "system_led.h"

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE		"mdp_app"

#define MDP_BEEP_FREQ		2755
#define MDP_INIT_BLINK_DELAY	100	/* Init done blink delay msec */
#define MDP_INIT_BEEP_DELAY	600	/* Beep time after parktronic on */

#define MDP_DIST_BEEP_NONE	150	/* Distance in centimeters */
#define MDP_DIST_BEEP_SLOW	90	/* Distance in centimeters */
#define MDP_DIST_BEEP_FAST	30	/* Distance in centimeters */

#define MDP_NO_DATA_STR		"    -.-m    "
#define MDP_DATA_TMPL_STR	"    %u.%um    "
#define MDP_STEP		30
#define MDP_STEP_CNT		9
#define MDP_STEP_STRLEN		4
#define MDP_STEP_STR								\
	{									\
		"\xF0\xF0\xF0\xF0", "\xF0\xF0\xF0\x3E", "\xF0\xF0\xF0 ",	\
		"\xF0\xF0\x3E ",    "\xF0\xF0  ",       "\xF0\x3E  ",		\
		"\xF0   ",          "\x3E   ",          "\x3E   "		\
	}

struct mdp_state {
	uint32_t curr;
	uint32_t prev;
};

static struct mdp_state ptronic_state;
static struct mdp_can dp_can, pjb_can;

static const char *dist_steps[] = MDP_STEP_STR;
static uint8_t mdp_buffer[MAZDA_DP_REG_NUM][MAZDA_DP_MSG_SIZE];

static void error_handler(void)
{
	mdp_can_bypass_on();

	__disable_irq();

	while(true) {

	}
}

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
	log_sys("System clock %lu MHz\r\n", MDP_CLOCK_FREQ_MHZ)
	log_sys("Software version: %s %s\r\n", MDP_APP_VERSION,
		MDP_APP_DEBUG ? "debug" : "release");
	log_sys("Board revision: %s\r\n", MDP_BOARD_REVISION);
	log_sys("Build date: %s\r\n", __TIMESTAMP__);
	log_sys("%s\r\n", line);
}

static void update_display(char *string)
{
	size_t string_len;
	static char dp_string[MAZDA_DP_CHAR_NUM];

	/* Prepare string for displaying: trim or add trailing spaces */
	string_len = strlen(string);
	if (string_len > MAZDA_DP_CHAR_NUM) {
		string_len = MAZDA_DP_CHAR_NUM;
	}
	memcpy(dp_string, string, string_len);
	memset(&dp_string[string_len], ' ', MAZDA_DP_CHAR_NUM - string_len);

	/**
	 * Fill display buffer
	 *
	 * Mazda 3 has 12-symbol LCD display, and it has two logical parts:
	 *	1. Left half of display with CAN bus address 0x290.
	 *	2. Right half of display with CAN bus address 0x291.
	 *
	 * To write data to the display, we need to
	 * send 2 data packets 8 bytes each.
	 *
	 * Start byte of each data packet is service byte and it is different
	 * for each packet:
	 *	1. 0xC0 - For the left half of display.
	 *	2. 0x85 - For the right half of display.
	 *
	 * Thus, effective payload of display is 14 bytes.
	 * Due to the fact that the display has only 12 characters to display,
	 * we need to place data in buffer in a certain way: in left half we
	 * will write first 7 bytes of our data with offset 1 (service byte);
	 * in right half we will write next 7 bytes of data, with an data offset
	 * not of 7 bytes, but 5.
	 *
	 * Example: Write string "Initializing" to the display
	 *	Index:        0      1     2     3     4     5     6     7
	 *	Left half:  [0xC0] ['I'] ['n'] ['i'] ['t'] ['i'] ['a'] ['l']
	 *	Right half: [0x85] ['a'] ['l'] ['i'] ['z'] ['i'] ['n'] ['g']
	 */
	mdp_buffer[MAZDA_DP_LHALF][0] = MAZDA_DP_LHALF_BYTE;
	mdp_buffer[MAZDA_DP_RHALF][0] = MAZDA_DP_RHALF_BYTE;

	memcpy(&mdp_buffer[MAZDA_DP_LHALF][1],
	       dp_string,
	       MAZDA_DP_MSG_SIZE - 1);
	memcpy(&mdp_buffer[MAZDA_DP_RHALF][1],
	       &dp_string[MAZDA_DP_CHAR_NUM - MAZDA_DP_MSG_SIZE + 1],
	       MAZDA_DP_MSG_SIZE - 1);
}

static void distance_to_string(struct ptronic_data *ptronic, char *string)
{
	const uint32_t dist_step = MDP_STEP;
	const uint32_t l_flag = 0x80000000, r_flag = 0x00008000;
	uint16_t main_dist, left_dist, right_dist;
	uint32_t dist_flag = 0;

	/* No data from sensors */
	if (!ptronic->valid) {
		sprintf(string, MDP_NO_DATA_STR);
		return;
	}

	/* Get minimum distance for left and right halfs */
	left_dist = MIN(ptronic->distance[MDP_SENSOR_A],
			ptronic->distance[MDP_SENSOR_B]);
	right_dist = MIN(ptronic->distance[MDP_SENSOR_C],
			 ptronic->distance[MDP_SENSOR_D]);
	main_dist = MIN(left_dist, right_dist);

	/* Write distance in meters */
	sprintf(string, MDP_DATA_TMPL_STR, (main_dist / 100),
		(main_dist % 100) / 10);

	/* Write arrows for the left and right halfs of the display */
	for (int i = 0; i < MDP_STEP_CNT; ++i) {
		if (!(dist_flag & l_flag) && left_dist <= (dist_step * i)) {
			for (int j = 0; j < MDP_STEP_STRLEN; j++) {
				string[j] = dist_steps[i][j];
			}

			dist_flag |= l_flag;
		}

		if (!(dist_flag & r_flag) && right_dist <= (dist_step * i)) {
			for (int j = MDP_STEP_STRLEN - 1; j >= 0; j--) {
				char *ch = &string[MAZDA_DP_CHAR_NUM - 1 - j];

				switch(dist_steps[i][j]) {
				case '\xF0':		/* Solid right arrow */
					*ch = '\xF1';	/* Solid left arrow */
					break;
				case '\x3E':		/* Usual right arrow */
					*ch = '\x3C';	/* Usual left arrow */
					break;
				}
			}

			dist_flag |= r_flag;
		}

		if (dist_flag & l_flag & r_flag)
			break;
	}
}

static mdp_beep_mode_t distance_to_beep(struct ptronic_data *ptronic)
{
	uint16_t main_dist, left_dist, right_dist;

	/* No data from sensors */
	if (!ptronic->valid)
		return MDP_BEEP_NONE;

	/* Get minimum distance for left and right halfs */
	left_dist = MIN(ptronic->distance[MDP_SENSOR_A],
			ptronic->distance[MDP_SENSOR_B]);
	right_dist = MIN(ptronic->distance[MDP_SENSOR_C],
			 ptronic->distance[MDP_SENSOR_D]);
	main_dist = MIN(left_dist, right_dist);

	if (main_dist >= MDP_DIST_BEEP_NONE) {
		return MDP_BEEP_NONE;
	} else if (main_dist < MDP_DIST_BEEP_NONE &&
		   main_dist >= MDP_DIST_BEEP_SLOW) {
		return MDP_BEEP_SLOW;
	} else if (main_dist < MDP_DIST_BEEP_SLOW &&
		   main_dist >= MDP_DIST_BEEP_FAST) {
		return MDP_BEEP_FAST;
	}

	return MDP_BEEP_CONST;
}

static bool get_bit_state_updated(uint8_t byte, uint8_t bit,
				  struct mdp_state *state)
{
	if ((byte & BIT(bit)) && !state->curr) {
		state->curr = true;
		state->prev = false;
		return true;
	}

	if (!(byte & BIT(bit)) && state->curr) {
		state->curr = false;
		state->prev = true;
		return true;
	}

	return false;
}

static void app_inited_blink(void)
{
	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);
	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);

	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);
	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);

	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);
	mdp_sysled_toggle();
	mdp_tm_msleep(MDP_INIT_BLINK_DELAY);
	mdp_sysled_off();
}

static void mdp_can_replace_data(struct mdp_can_msg *msg)
{
	static const uint8_t dp_misc_symb0[] = MAZDA_DP_MISC_SYMB0_BIT;
	static const uint8_t dp_misc_symb1[] = MAZDA_DP_MISC_SYMB1_BIT;
	static const uint8_t dp_misc_symb2[] = MAZDA_DP_MISC_SYMB2_BIT;

	switch(msg->id) {
	case MAZDA_DP_MISC_SYMB_ID:
		/* Turn off all active symbols */
		/* CD IN/MD IN/ST/Dolby/RPT/RDM/AF symbols  */
		for (int i = 0; i < ARRAY_SIZE(dp_misc_symb0); i++) {
			RESET_BIT(msg->data[MAZDA_DP_MISC_SYMB0],
				  dp_misc_symb0[i]);
		}

		/* Turn off all active symbols */
		/* PTY/TA/TP/AUTO-M symbols */
		for (int i = 0; i < ARRAY_SIZE(dp_misc_symb1); i++) {
			RESET_BIT(msg->data[MAZDA_DP_MISC_SYMB1],
				  dp_misc_symb1[i]);
		}

		/* Turn off all active symbols */
		/* "":"/"'"/"." symbols */
		for (int i = 0; i < ARRAY_SIZE(dp_misc_symb2); i++) {
			RESET_BIT(msg->data[MAZDA_DP_MISC_SYMB2],
				  dp_misc_symb2[i]);
		}

		break;
	case MAZDA_DP_LHALF_ID:
		memcpy(msg->data, mdp_buffer[MAZDA_DP_LHALF], msg->size);
		break;
	case MAZDA_DP_RHALF_ID:
		memcpy(msg->data, mdp_buffer[MAZDA_DP_RHALF], msg->size);
		break;
	default:
		return;
	}
}

static void mdp_can_transfer_pjb_to_dp_replace_dp(void)
{
	int ret;

	ret = mdp_can_read(&pjb_can);
	if (ret > 0) {
		mdp_can_replace_data(&pjb_can.msg);

		memcpy(&dp_can.msg, &pjb_can.msg, sizeof(dp_can.msg));

		ret = mdp_can_write(&dp_can);
		if (ret < 0) {
			log_err("MDP CAN DP write failed!\r\n");
			error_handler();
		}
	} else if (ret < 0) {
		log_err("MDP CAN PJB read failed!\r\n");
		error_handler();
	}
}

void mdp_init(void)
{
	int ret;

	log_app_info();

	dp_can = mdp_get_can_spi_interface();
	pjb_can = mdp_get_can_hal_interface();

	/* Bypass all CAN packets through while board is not inited */
	mdp_can_bypass_on();
	mdp_sysled_off();

#if (MDP_BEEPER_ENABLED == 1)
	if (!mdp_beeper_init(MDP_BEEP_FREQ))
		log_err("Beeper init failed\r\n");

	if (!mdp_beeper_set_mode(MDP_BEEP_NONE))
		log_err("Beeper set mode failed!\r\n");
#endif

	ret = mdp_can_start(&dp_can);
	if (ret) {
		log_err("DP CAN (SPI) start failed!\r\n");
		goto exit_error;
	}

	ret = mdp_can_start(&pjb_can);
	if (ret) {
		log_err("PJB CAN (HAL) start failed!\r\n");
		goto exit_error;
	}

	app_inited_blink();

	return;

exit_error:
	error_handler();
}

void mdp_run(void)
{
	bool state_updated;

	state_updated = get_bit_state_updated(mdp_ptronic_is_enabled(), 0,
					      &ptronic_state);
	if (state_updated) {
		if (ptronic_state.curr) {
			log_info("Parktronic enabled!\r\n");

			mdp_beeper_set_mode(MDP_BEEP_CONST);
			mdp_beeper_beep();

			mdp_tm_msleep(MDP_INIT_BEEP_DELAY);

			mdp_beeper_set_mode(MDP_BEEP_NONE);
			mdp_beeper_beep();

			mdp_can_bypass_off();
		} else {
			log_info("Parktronic disabled!\r\n");

			mdp_can_bypass_on();

			mdp_beeper_set_mode(MDP_BEEP_NONE);
			mdp_beeper_beep();
		}
	}

	if (ptronic_state.curr) {
		char dist_str[MAZDA_DP_CHAR_NUM * 2];
		struct ptronic_data data = ptronic_read_data();

		mdp_beeper_set_mode(distance_to_beep(&data));
		mdp_beeper_beep();

		distance_to_string(&data, dist_str);
		update_display(dist_str);

		mdp_can_transfer_pjb_to_dp_replace_dp();
	}
}
