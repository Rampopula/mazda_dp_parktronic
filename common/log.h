/**
 * @file       log.h
 * @brief      Simple logging API using printf() function.
 *
 * @date       August 17, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */

#ifndef __MDP_LOG_H__
#define __MDP_LOG_H__

#include <stdio.h>

#include "time.h"

#define LOG_LEVEL_ERROR	0
#define LOG_LEVEL_SYS	0
#define LOG_LEVEL_INFO	1
#define LOG_LEVEL_DEBUG	2

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ERROR
#endif

#ifdef MDP_MODULE
#undef MDP_MODULE
#endif
#define MDP_MODULE ""

#define __log(module, level, fmt, args...) \
	do { \
		if (level <= LOG_LEVEL) { \
			struct mdp_timestamp ts = MDP_TIMESTAMP(); \
			printf("[%lu.%03lu] " module ": " fmt, \
				ts.sec, ts.msec, ##args); \
		} \
	} while (0);

#define __log_debug(module, level, fmt, args...) \
	do { \
		if (level <= LOG_LEVEL) { \
			struct mdp_timestamp ts = MDP_TIMESTAMP(); \
			printf("[%lu.%03lu] " module "(%s:%d): " fmt, ts.sec, \
				ts.msec, __FUNCTION__, __LINE__, ##args); \
		} \
	} while (0);

#define log_sys(fmt, args...) \
	__log(MDP_MODULE, LOG_LEVEL_SYS, fmt, ##args)

#if (LOG_LEVEL == LOG_LEVEL_DEBUG)
#define log_err(fmt, args...) \
	__log_debug(MDP_MODULE, LOG_LEVEL_ERROR, fmt, ##args)
#define log_std(fmt, args...) \
	__log_debug(MDP_MODULE, LOG_LEVEL_INFO, fmt, ##args)
#define log_dbg(fmt, args...) \
	__log_debug(MDP_MODULE, LOG_LEVEL_DEBUG, fmt, ##args)
#else
#define log_err(fmt, args...) \
	__log(MDP_MODULE, LOG_LEVEL_ERROR, fmt, ##args)
#define log_std(fmt, args...) \
	__log(MDP_MODULE, LOG_LEVEL_INFO, fmt, ##args)
#define log_dbg(fmt, args...)
#endif

#endif /* __MDP_LOG_H__ */
