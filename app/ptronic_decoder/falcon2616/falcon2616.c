/**
 * @file       falcon2616.c
 * @brief      Falcon 2616 Parktronic decoder API implementation.
 *
 * @date       April 29, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#include "falcon2616.h"
#include "falcon2616_gpio_intf.h"
#include "common.h"
#include "time.h"

#include <string.h>

#define read_gpio()		f2616_intf_gpio_read()

#define F2616_SNS_OK		1
#define F2616_SNS_INVAL		0x80000000

#define F2616_SNS_ID_BIT	8
#define F2616_SNS_STAT_BIT	10
#define F2616_SNS_INFO_MASK	0x3

#define F2616_GET_SNS(x)                                                       \
	(uint8_t)((x >> F2616_SNS_ID_BIT) & F2616_SNS_INFO_MASK)
#define F2616_GET_SNS_STAT(x)                                                  \
	(uint8_t)((x >> F2616_SNS_STAT_BIT) & F2616_SNS_INFO_MASK)

#define PTRONIC_FRAME_SIZE		16 /* 16 bits of data */
#define PTRONIC_START_BIT_TIME_MIN	800 /* microseconds */
#define PTRONIC_START_BIT_TIME_MAX	900 /* microseconds */
#define PTRONIC_HIGH_BIT_TIME_MIN	450 /* microseconds */
#define PTRONIC_HIGH_BIT_TIME_MAX	600 /* microseconds */

#define PTRONIC_READY_MASK 0xF /* Four bits for every sensor */

typedef enum {
	F2616_DIST_0p0m = 255,

	/**
        * From 252 to 246, 0.1m step
        */
	F2616_DIST_0p3m = 252,
	F2616_DIST_0p9m = 246,

	/**
        * From 239 to 230, 0.1m step
        */
	F2616_DIST_1p0m = 239,
	F2616_DIST_1p9m = 230,

	/**
        * From 223 to 218, 0.1m step
        */
	F2616_DIST_2p0m = 223,
	F2616_DIST_2p5m = 218,
} f2616_distance_t;

static void f2616_gpio_irq(void);
void (*__F2616_GPIO_IRQ_CB)(void) = f2616_gpio_irq;

static struct mdp_time tm;
static bool read_frame;
static int read_data;
static uint16_t frame;
static volatile uint32_t ptronic_ready;
static volatile uint32_t distance_cm[F2616_SNS_CNT];
static struct f2616_distance f2616_distance;

static void f2616_convert_distance(uint16_t data)
{
	uint8_t idx = F2616_GET_SNS(data), distance = data;

	if (F2616_GET_SNS_STAT(data) != F2616_SNS_OK) {
		distance_cm[idx] |= F2616_SNS_INVAL;
		return;
	}

	if (distance == F2616_DIST_0p0m)
		distance_cm[idx] = 0;
	else if (IN_RANGE(distance, F2616_DIST_0p9m, F2616_DIST_0p3m))
		distance_cm[idx] = (F2616_DIST_0p0m - distance) * 10;
	else if (IN_RANGE(distance, F2616_DIST_1p9m, F2616_DIST_1p0m))
		distance_cm[idx] = (F2616_DIST_1p0m - distance) * 10 + 100;
	else if (IN_RANGE(distance, F2616_DIST_2p5m, F2616_DIST_2p0m))
		distance_cm[idx] = (F2616_DIST_2p0m - distance) * 10 + 200;
	else
		return;

	ptronic_ready |= 1 << idx;
}

static void f2616_gpio_irq(void)
{
	uint32_t bit_time;

	/**
         * If we read high level on GPIO, then we have rising edge,
         * rising edge means start of data transfer.
         */
	if (read_gpio()) {
		/* Begin to measure time and detect start bit */
		mdp_tm_measure_start(&tm);
		read_frame = true;
	/**
         * If we read low level on GPIO,  then we have falling edge,
         * falling edge means end of data transfer.
         */
	} else if (read_frame) {
		/* Finish measuring time */
		mdp_tm_measure_stop(&tm);
		bit_time = mdp_tm_measure_get_us(&tm);
		read_frame = false;

		/* If we read the start bit */
		if (!read_data) {
			/* Detect start bit */
			read_data =
				IN_RANGE(bit_time, PTRONIC_START_BIT_TIME_MIN,
					 PTRONIC_START_BIT_TIME_MAX);
		/* If we read the frame data */
		} else {
			uint8_t bit =
				IN_RANGE(bit_time, PTRONIC_HIGH_BIT_TIME_MIN,
					 PTRONIC_HIGH_BIT_TIME_MAX);
			frame |= bit << (read_data - 1);
			read_data++;

			/* If we read all bits from the frame */
			if (read_data == PTRONIC_FRAME_SIZE + 1) {
				f2616_convert_distance(frame);
				frame = 0;
				read_data = false;
			}
		}
	}
}

bool f2616_ready(void)
{
	return (ptronic_ready & PTRONIC_READY_MASK);
}

struct f2616_distance *f2616_read_distance(void)
{
	if (distance_cm[F2616_SNS_A] & F2616_SNS_INVAL &&
	    distance_cm[F2616_SNS_B] & F2616_SNS_INVAL &&
	    distance_cm[F2616_SNS_C] & F2616_SNS_INVAL &&
	    distance_cm[F2616_SNS_D] & F2616_SNS_INVAL) {
		f2616_distance.valid_data = false;
		return &f2616_distance;
	}

	f2616_distance.valid_data = true;
	f2616_distance.cm[F2616_SNS_A] = distance_cm[F2616_SNS_A];
	f2616_distance.cm[F2616_SNS_B] = distance_cm[F2616_SNS_B];
	f2616_distance.cm[F2616_SNS_C] = distance_cm[F2616_SNS_C];
	f2616_distance.cm[F2616_SNS_D] = distance_cm[F2616_SNS_D];

	return &f2616_distance;
}
