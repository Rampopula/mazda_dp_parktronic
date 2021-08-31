/**
 * @file       mdp.h
 * @brief      Mazda 3/5/6 specific CAN Bus definitions.
 *
 *             Thanks to Madox from http://www.madox.net who first
 *             investigated Mazda 3 CAN Bus.
 *             Thanks to Emanuele from comments on http://www.madox.net who
 *             sniffed the Mazda CAN HS and CAN MS busses and created
 *             a great message and data tables.
 *
 *             Reference links:
 *                 1. Mazda CAN Bus
 *                    http://www.madox.net/blog/projects/mazda-can-bus/
 *                 3. Reverse Engineering the Mazda CAN Bus – Part 1
 *                    http://www.madox.net/blog/2008/11/17/reverse-engineering-
 *                    the-mazda-can-bus-part-1/comment-page-1/
 *                 4. Reverse Engineering the Mazda CAN Bus – Part 2
 *                    http://www.madox.net/blog/2009/10/24/reverse-engineering-
 *                    the-mazda-can-bus-%E2%80%93-part-2/
 *                 5. Emanuele CAN HS BUS table
 *                    https://goo.gl/l1HpVE
 *                 6. Emanuele CAN MS BUS table
 *                    https://goo.gl/Roh9ur
 *
 * @date       August 31, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MAZDA_DP_PARKTRONIC_H__
#define __MAZDA_DP_PARKTRONIC_H__

#include "boardinfo.h"
#include "log.h"

/* Common */
#define MAZDA_MAX_MSG_SIZE	8 /* CAN Bus maximum message size */

/* Display */
#define MAZDA_DP_LHALF		0 /* ID of the display left half */
#define MAZDA_DP_RHALF		1 /* ID of the display right half */
#define MAZDA_DP_REG_NUM	2 /* Number of display registers */
#define MAZDA_DP_MSG_SIZE	8 /* Display message size */
#define MAZDA_DP_CHAR_NUM	12 /* Maximum number of characters on display */
#define MAZDA_DP_LHALF_ID	0x290 /* Message ID of the display left half */
#define MAZDA_DP_RHALF_ID	0x291 /* Message ID of the display right half */
#define MAZDA_DP_LHALF_BYTE	0xC0 /* 1st byte of display left half packet */
#define MAZDA_DP_RHALF_BYTE	0x85 /* 1st byte of display right half packet */

/* Status */
#define MAZDA_STAT_ID		0x433 /* ID of the status message */
#define MAZDA_STAT_RGEAR_BYTE	3 /* Reverse gear byte number */
#define MAZDA_STAT_RGEAR_BIT	1 /* Reverse gear bit number */

/**
 * @brief Initialize Mazda Display Parktronic application.
 *        This function must be called before the main loop.
 */
void mdp_init(void);

/**
 * @brief Run Mazda Display Parktronic application.
 *        This function must be called constantly in the main loop.
 */
void mdp_run(void);

#endif /* __MAZDA_DP_PARKTRONIC_H__ */
