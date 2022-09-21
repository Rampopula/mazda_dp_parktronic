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

/*
 * Display
 *     Display update time is about 4 FPS (every 250 ms)
 */
#define MAZDA_DP_MISC_SYMB_ID	0x28F /* ID of the display misc symbols */
#define MAZDA_DP_MISC_SYMB0	0 /* CD IN/MD IN/ST/Dolby/RPT/RDM/AF symbols */
#define MAZDA_DP_MISC_SYMB1	1 /* PTY/TA/TP/AUTO-M symbols */
#define MAZDA_DP_MISC_SYMB2	3 /* "":"/"'"/"." symbols */
#define MAZDA_DP_MISC_SYMB0_MSK 0x80 /* Dispay sysmbols mask */
#define MAZDA_DP_MISC_SYMB1_MSK 0x0F /* Dispay sysmbols mask */
#define MAZDA_DP_MISC_SYMB2_MSK 0xC9 /* Dispay sysmbols mask */
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
#define MAZDA_STAT_HBRAKE_BYTE	3 /* Hand brake byte number */
#define MAZDA_STAT_HBRAKE_BIT	0 /* Hand brake bit number */
#define MAZDA_STAT_FLDOOR_BYTE	0 /* Front left door byte number */
#define MAZDA_STAT_FLDOOR_BIT	7 /* Front left door bit number */

/* ASCII Symbols */
#define DP_ARR_RIGHT_SYMB	"\x1C"
#define DP_ARR_LEFT_SYMB	"\x1D"
#define DP_ARR_UP_SYM		"\x1E"
#define DP_ARR_DOWN_SYMB	"\x1F"
#define DP_LITTLE_1_SYMB	"\x9E"
#define DP_LITTLE_3_SYMB	"\xAE"
#define DP_CENTER_DOT_SYMB	"\xA5"
#define DP_JAPAN_SMILE_SYMB	"\xBC"
#define DP_CELSIUS_SYMB		"\xDF"
#define DP_BLACK_1_SYMB		"\xE1"
#define DP_BLACK_2_SYMB		"\xE2"
#define DP_BLACK_3_SYMB		"\xE3"
#define DP_BLACK_4_SYMB		"\xE4"
#define DP_BLACK_5_SYMB		"\xE5"
#define DP_BLACK_6_SYMB		"\xE6"

#define DP_REGULAR_1_SYMB	"\xE7"
#define DP_REGULAR_2_SYMB	"\xE8"
#define DP_REGULAR_3_SYMB	"\xE9"
#define DP_REGULAR_4_SYMB	"\xEA"
#define DP_REGULAR_5_SYMB	"\xEB"
#define DP_REGULAR_6_SYMB	"\xEC"

#define DP_TRIANGLE_UP_SYMB	"\xED"
#define DP_TRIANGLE_DOWN_SYMB	"\xEE"
#define DP_TRIANGLE_UPDOWN_SYMB	"\xEF"
#define DP_TRIANGLE_RIGHT_SYMB	"\xF0"
#define DP_TRIANGLE_LEFT_SYMB	"\xF1"

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
