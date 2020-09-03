/**
 * RTC Library
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#pragma once

#include "inc.h"

/**
 * Date time structure
 */
typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
} RtcDateTime;

/**
 * Initialize RTC
 */
void PCF8563_init();

/**
 * Set Date Time
 *
 * @param value Date & Time
 * @return 0 on success, else error code
 */
uint8_t PCF8563_write(RtcDateTime* value);

/**
 * RTC Read Date & Time
 *
 * @param value Value output
 * @return 0 on success, else error code
 */
uint8_t PCF8563_read(RtcDateTime* value);
