/**
 * @file       common.h
 * @brief      Common defines.
 *
 * @date       April 30, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_COMMON_H__
#define __MDP_COMMON_H__

#define BIT(x)			(1 << x)

#define MAX(x, y)		(((x) > (y)) ? (x) : (y))
#define MIN(x, y)		(((x) < (y)) ? (x) : (y))

#define IN_RANGE(a, x, y)	((a) >= (x) && (a) <= (y))

#endif /* __MDP_COMMON_H__ */
