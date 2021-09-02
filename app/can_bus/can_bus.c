#include "can_bus.h"
#include "log.h"

#include "can_hal.h"
#include "can_spi.h"

#include <errno.h>

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE "can_bus"

int mdp_can_start(struct mdp_can *can)
{
	if (!can || !can->ops.start) {
		log_err("Invalid arguments: can = %p, ops.start = %p\r\n",
			can, can->ops.start);
		return -EINVAL;
	}

	return can->ops.start();
}

int mdp_can_stop(struct mdp_can *can)
{
	if (!can || !can->ops.stop) {
		log_err("Invalid arguments: can = %p, ops.stop = %p\r\n",
			can, can->ops.stop);
		return -EINVAL;
	}

	return can->ops.stop();
}

int mdp_can_read(struct mdp_can *can)
{
	int ret = 0;

	if (!can || !can->ops.read) {
		log_err("Invalid arguments: can = %p, ops.read = %p\r\n",
			can, can->ops.read);
		return -EINVAL;
	}

	ret = can->ops.read(&can->msg.id, can->msg.data, &can->msg.size);
	if (ret <= 0)
		return ret;

	if (can->sniffer)
		can->sniffer->sniff(can->sniffer, &can->msg);

	return ret;
}

int mdp_can_write(struct mdp_can *can)
{
	if (!can || !can->ops.write) {
		log_err("Invalid arguments: can = %p, ops.write = %p\r\n",
			can, can->ops.write);
		return -EINVAL;
	}

	return can->ops.write(can->msg.id, can->msg.data, can->msg.size);
}

int mdp_can_attach_sniffer(struct mdp_can *can,
			   struct mdp_can_sniffer *sniffer)
{
	if (!can || !sniffer) {
		log_err("Invalid arguments: can = %p, sniffer = %p\r\n",
			can, sniffer);
		return -EINVAL;
	}

	can->sniffer = sniffer;

	return 0;
}

struct mdp_can mdp_get_can_hal_interface(void)
{
	struct mdp_can intf = {
		.sniffer = NULL,
		.ops = {
			.start = mdp_can_hal_start,
			.stop = mdp_can_hal_stop,
			.read = mdp_can_hal_read,
			.write = mdp_can_hal_write
		}
	};

	return intf;
}

 struct mdp_can mdp_get_can_spi_interface(void)
{
	struct mdp_can intf = {
		.sniffer = NULL,
		.ops = {
			.start = mdp_can_spi_start,
			.stop = mdp_can_spi_stop,
			.read = mdp_can_spi_read,
			.write = mdp_can_spi_write
		}
	};

	return intf;
}
