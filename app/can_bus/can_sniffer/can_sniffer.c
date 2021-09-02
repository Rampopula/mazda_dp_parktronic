#include "can_sniffer.h"
#include "log.h"

#include <string.h>
#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "can_bus_sniffer"

static uint32_t sniffer_num;
static struct mdp_can_sniffer sniffer[MDP_CAN_SNIFFER_MAX_NUM];

static void sniff(struct mdp_can_sniffer *sniffer, struct mdp_can_msg *msg)
{
	int i;

	if (!sniffer || !msg) {
		log_err("Invalid arguments: sniffer = %p, msg = %p\r\n",
			sniffer, msg);
		return;
	}

	for (i = 0; i < sniffer->sniff_num; i++) {
		struct mdp_can_sniff_ctx *sniff = &sniffer->sniffs[i];

		if (msg->id != sniff->id)
			continue;

		if (msg->size - 1 < sniff->byte) {
			log_dbg("Sniff id match, but size is less!\r\n");
			continue;
		}

		if ((msg->data[sniff->byte] & sniff->mask) == sniff->cond) {
			sniff->callback(msg, sniff);
			break;
		}
	}
}

struct mdp_can_sniffer *mdp_can_get_sniffer(void)
{
	struct mdp_can_sniffer *_sniffer;

	if (sniffer_num >= MDP_CAN_SNIFFER_MAX_NUM) {
		log_err("Sniffer max number reached (max = %d)!\r\n",
			MDP_CAN_SNIFFER_MAX_NUM);
		return NULL;
	}

	_sniffer = &sniffer[sniffer_num++];
	_sniffer->sniff_num = 0;
	_sniffer->sniff = sniff;

	return _sniffer;
}

int mdp_can_sniffer_add_sniff(struct mdp_can_sniffer *sniffer,
			      struct mdp_can_sniff_ctx *sniff)
{
	if (!sniffer || !sniff) {
		log_err("Invalid arguments: sniffer = %p, sniff = %p\r\n",
			sniffer, sniff);
		return -EINVAL;
	}

	if (!sniff->callback) {
		log_err("Invalid sniff callback!\r\n")
		return -EINVAL;
	}

	if (sniffer->sniff_num >= MDP_CAN_SNIFFER_MAX_SNIFFS) {
		log_err("Too many notifications registred (max = %d)!\r\n",
			MDP_CAN_SNIFFER_MAX_SNIFFS);
		return -ENOMEM;
	}

	memcpy(&sniffer->sniffs[sniffer->sniff_num++], sniff,
	       sizeof(struct mdp_can_sniff_ctx));

	return 0;
}
