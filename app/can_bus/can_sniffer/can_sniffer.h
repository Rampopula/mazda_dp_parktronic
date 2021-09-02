#ifndef __MDP_CAN_SNIFFER_H__
#define __MDP_CAN_SNIFFER_H__

#include <stdint.h>

#include "can_bus_def.h"

#define MDP_CAN_SNIFFER_MAX_NUM		2
#define MDP_CAN_SNIFFER_MAX_SNIFFS	4

struct mdp_can_sniff_ctx {
	uint32_t id;	/* Packet ID */
	uint32_t byte;	/* Packet byte number */
	uint8_t mask;	/* Mask that will be applied to byte */
	uint8_t cond;	/* Condition to call sniffer callback */
	void(*callback)(struct mdp_can_msg *, struct mdp_can_sniff_ctx *);
};

struct mdp_can_sniffer {
	uint32_t sniff_num;
	void(*sniff)(struct mdp_can_sniffer *, struct mdp_can_msg *);
	struct mdp_can_sniff_ctx sniffs[MDP_CAN_SNIFFER_MAX_SNIFFS];
};

struct mdp_can_sniffer *mdp_can_get_sniffer(void);

int mdp_can_sniffer_add_sniff(struct mdp_can_sniffer *sniffer,
			      struct mdp_can_sniff_ctx *sniff);

#endif /* __MDP_CAN_SNIFFER_H__ */
