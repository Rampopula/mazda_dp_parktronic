/**
 * @file       hexdump.h
 * @brief      Simple hexdump implentation, prints hex in following format:
 *                 Printing hexdump of 'Simple hexdump implementation.':
 *                     00000000  53 69 6d 70 6c 65 20 68   |Simple h|
 *                     00000008  65 78 64 75 6d 70 20 69   |exdump i|
 *                     00000010  6d 70 6c 65 6d 65 6e 74   |mplement|
 *                     00000018  61 74 69 6f 6e 2e 00 00   |ation.  |
 *
 * @date       April 7, 2021
 * @author     Eduard Chaika <rampopula@gmail.com>
 * @copyright  Copyright (c) 2021 Eduard Chaika
 */
#ifndef __MAZDA_DP_HEXDUMP_H__
#define __MAZDA_DP_HEXDUMP_H__

#include <stdio.h>
#include <stdint.h>

#define HEXDUMP_DEF_GROUP_BY 16

/**
 * @brief Print hexdump of byte array.
 *
 * @param [in] caption Caption string of hexdump call, <NULL> if disabled.
 * @param [in] group_by Number of bytes in row, 0 - to set default value (16).
 * @param [in] data Byte array to dump.
 * @param [in] size Amount of data to dump.
 */
static inline void hexdump(const char *caption, uint32_t group_by,
			   uint8_t *data, uint32_t size)
{
	int rows;

	if (!group_by) {
		group_by = HEXDUMP_DEF_GROUP_BY;
	}

	/* Calculate number of rows to display, add trailing zeros if needed */
	rows = (size / group_by) + !!(size - ((size / group_by) * group_by));

	printf("\r\n");

	if (caption)
		printf("Printing hexdump of '%s':\r\n", caption);

	for (int row = 0; row < rows; ++row) {
		/* Print hex address */
		printf("%08lx  ", (row * group_by));

		/* Print hex represantation */
		for (int col = 0; col < group_by; ++col) {
			if ((row * group_by) + col >= size)
				printf("%02x ", '\0');
			else
				printf("%02x ", data[(row * group_by) + col]);
		}

		/* Print ASCII represantation */
		printf("  |");
		for (int col = 0; col < group_by; ++col) {
			if ((row * group_by) + col >= size)
				printf(" ");
			else
				printf("%c", data[(row * group_by) + col]);
		}
		printf("|\r\n");
	}
}

#endif /* __MAZDA_DP_HEXDUMP_H__ */
