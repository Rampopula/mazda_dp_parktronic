#include "beeper.h"
#include "beeper_intf.h"

#include "log.h"
#include "common.h"

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "mdp_beeper"

#define MDP_BEEP_MIN_FREQ	1000	/* Minimal frequency is 1 KHz */
#define MDP_BEEP_MAX_FREQ	10000	/* Maximal frequency is 10 KHz */

#define beeper_on()		mdp_intf_beeper_start()
#define beeper_off()		mdp_intf_beeper_stop()
#define beeper_set_freq(x)	mdp_intf_beeper_set_freq(x)

static bool beeper_inited;
static mdp_beep_mode_t beep_mode;
static struct mdp_timestamp beep_ts;

static inline char *mode_to_str(mdp_beep_mode_t mode)
{
	switch (mode) {
	case MDP_BEEP_NONE:
		return "MDP_BEEP_NONE";
	case MDP_BEEP_SLOW:
		return "MDP_BEEP_SLOW";
	case MDP_BEEP_FAST:
		return "MDP_BEEP_FAST";
	case MDP_BEEP_CONST:
		return "MDP_BEEP_CONST";
	default:
		return "invalid mode";
	}
}

static inline void beep_toggle(void)
{
	static bool beep_on = false;

	if (beep_on) {
		beeper_off();
		beep_on = false;
	} else {
		beeper_on();
		beep_on = true;
	}
}

static inline bool beep_time_elapsed(uint32_t time_ms)
{
	if (MDP_TIMESTAMP.ticks - beep_ts.ticks >= time_ms) {
		beep_ts = MDP_TIMESTAMP;
		return true;
	}

	return false;
}

bool mdp_beeper_init(uint32_t freq)
{
	if (!IN_RANGE(freq, MDP_BEEP_MIN_FREQ, MDP_BEEP_MAX_FREQ)) {
		log_err("Invalid freq = %lu, must be in range [%u,%u]\r\n",
			freq, MDP_BEEP_MIN_FREQ, MDP_BEEP_MAX_FREQ);
		return false;
	}

	beeper_set_freq(freq);

	beep_ts = MDP_TIMESTAMP;

	beeper_inited = true;

	log_dbg("Beeper inited with frequency %lu Hz\r\n", freq);
	return true;
}

void mdp_beeper_beep(void)
{
	if (!beeper_inited)
		return;

	switch (beep_mode) {
	case MDP_BEEP_NONE:
		beeper_off();
		break;
	case MDP_BEEP_SLOW:
		if (beep_time_elapsed(MDP_BEEP_SLOW_INTERVAL))
			beep_toggle();
		break;
	case MDP_BEEP_FAST:
		if (beep_time_elapsed(MDP_BEEP_FAST_INTERVAL))
			beep_toggle();
		break;
	case MDP_BEEP_CONST:
		beeper_on();
		break;
	default:
		break;
	}
}

bool mdp_beeper_set_mode(mdp_beep_mode_t mode)
{
	if (!IN_RANGE(mode, MDP_BEEP_NONE, MDP_BEEP_CONST)) {
		log_err("Invalid beeper mode = %d!\r\n", mode);
		return false;
	}

	if (!beeper_inited)
		log_dbg("Beeper not inited!\r\n");

	beep_mode = mode;

	log_dbg("Beeper mode changed to %s\r\n", mode_to_str(mode));
	return true;
}
