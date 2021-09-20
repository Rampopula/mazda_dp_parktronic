#include "sequences.h"
#include "log.h"

#include <string.h>
#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "mdp_sequences"

#define MDP_SEQUENCE_MIN_TIME_MS	1000

static uint32_t sequence_num;
static struct mdp_sequence sequence[MDP_SEQ_MAX_NUM];

static enum seq_state_transition get_transition(struct mdp_seq_state *state)
{
	if (*state->data && !*state->prev_data)
		return SEQ_TRANSITION_0_TO_1;
	else if (!*state->data && *state->prev_data)
		return SEQ_TRANSITION_1_TO_0;
	else
		return SEQ_TRANSITION_NONE;
}

static void seq_reset(struct mdp_sequence *seq)
{
	int i;

	for (i = 0; i < seq->state_idx; i++)
		seq->state[i].passed = false;

	seq->state_idx = 0;
	seq->tm = MDP_TIMESTAMP;
}

static inline bool seq_time_elapsed(struct mdp_sequence *seq)
{
	return (MDP_TIMESTAMP.ticks - seq->tm.ticks >= seq->seq_time_ms);
}

struct mdp_sequence *mdp_get_new_sequence(uint32_t seq_time_ms,
					  void(*sequence_cb)(void*), void *data)
{
	if (sequence_num >= MDP_SEQ_MAX_NUM) {
		log_err("Sequence max number reached (max = %d)!\r\n",
			MDP_SEQ_MAX_NUM);
		return NULL;
	}

	if (!sequence_cb) {
		log_err("Invalid sequence callback!\r\n");
		return NULL;
	}

	if (seq_time_ms < MDP_SEQUENCE_MIN_TIME_MS) {
		log_err("Invalid sequence time (min = %d)!\n",
			MDP_SEQUENCE_MIN_TIME_MS);
		return NULL;
	}

	sequence[sequence_num].tm = MDP_TIMESTAMP;
	sequence[sequence_num].seq_time_ms = seq_time_ms;
	sequence[sequence_num].sequence_cb = sequence_cb;
	sequence[sequence_num].data = data;

	return &sequence[sequence_num++];
}

int mdp_seq_add_state(struct mdp_sequence *seq,
		      enum seq_state_transition transition,
		      uint32_t *data, uint32_t *prev_data)
{
	if (!seq || !data) {
		log_err("Invalid arguments: seq = %p, data = %p\r\n",
			seq, data);
		return -EINVAL;
	}

	if (transition != SEQ_TRANSITION_0_TO_1 &&
	    transition != SEQ_TRANSITION_1_TO_0) {
		log_err("Invalid transition = %d\r\n", transition);
		return -EINVAL;
	}

	if (seq->state_num >= MDP_SEQ_STATE_MAX_NUM) {
		log_err("Too many states registred (max = %d)!\r\n",
			MDP_SEQ_STATE_MAX_NUM);
		return -EFAULT;
	}

	if (!data) {
		log_err("Invalid state data!\r\n");
		return -EINVAL;
	}

	seq->state[seq->state_num].passed = false;
	seq->state[seq->state_num].data = data;
	seq->state[seq->state_num].prev_data = prev_data;
	seq->state[seq->state_num].transition = transition;

	seq->state_num++;
	return 0;
}

void mdp_sequence_process(void)
{
	int i, j;

	for (i = 0; i < sequence_num; i++) {
		struct mdp_sequence *seq = &sequence[i];

		if (seq_time_elapsed(seq))
			seq_reset(seq);

		for (j = 0; j < seq->state_num; j++) {
			struct mdp_seq_state *state = &seq->state[j];

			if (state->passed)
				continue;

			if (get_transition(state) != state->transition)
				break;

			state->passed = true;
			seq->state_idx++;
		}

		if (seq->state_num && seq->state_idx == seq->state_num) {
			seq->sequence_cb(seq->data);
			seq_reset(seq);
		}
	}
}
