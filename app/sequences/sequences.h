/**
 * @file       sequences.h
 * @brief      Sequence module allows to track the states of variables and
 *             react to their changes under the condition of a certain sequence.
 *
 * @date       September 2, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_SEQUENCES_H__
#define __MDP_SEQUENCES_H__

#include <stdbool.h>
#include <stdint.h>

#include "time.h"

#define MDP_SEQ_MAX_NUM		4
#define MDP_SEQ_STATE_MAX_NUM	8

enum seq_state_transition {
	SEQ_TRANSITION_NONE = 0,
	SEQ_TRANSITION_0_TO_1,
	SEQ_TRANSITION_1_TO_0
};

struct mdp_seq_state {
	bool passed;
	enum seq_state_transition transition;
	uint32_t *data;
	uint32_t *prev_data;
};

struct mdp_sequence {
	uint32_t seq_time_ms;
	uint32_t state_idx;
	uint32_t state_num;
	struct mdp_seq_state state[MDP_SEQ_STATE_MAX_NUM];
	struct mdp_timestamp tm;
	void(*sequence_cb)(void*);
	void *data;
};

struct mdp_sequence *mdp_get_new_sequence(uint32_t seq_time_ms,
					  void(*seq_cb)(void*), void *data);

int mdp_seq_add_state(struct mdp_sequence *seq,
		      enum seq_state_transition transition,
		      uint32_t *data, uint32_t *prev_data);

void mdp_sequence_process(void);

#endif /* __MDP_SEQUENCES_H__*/
