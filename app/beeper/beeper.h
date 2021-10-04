/**
 * @file       beeper.h
 * @brief      Beeper API for signaling the work of parking sensors.
 *
 * @date       August 20, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_BEEPER_H__
#define __MDP_BEEPER_H__

#include <stdbool.h>
#include <stdint.h>

#define MDP_BEEP_SLOW_INTERVAL	300
#define MDP_BEEP_FAST_INTERVAL	100

typedef enum {
	MDP_BEEP_NONE = 0,
	MDP_BEEP_SLOW,
	MDP_BEEP_FAST,
	MDP_BEEP_CONST
} mdp_beep_mode_t;

bool mdp_beeper_init(uint32_t freq);

void mdp_beeper_beep(void);

bool mdp_beeper_set_mode(mdp_beep_mode_t mode);

#endif /* __MDP_BEEPER_H__*/
