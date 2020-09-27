/**
 * Clock Hardware Control (Shift Register Matrix)
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#pragma once

#include "lib/inc.h"

/**
 * Additional bitmask
 */
extern uint8_t g_AdditionalLEDs;

/**
 * Initialize Clock Display
 */
void ClockDisplay_init();

/**
 * Clear the current buffer
 */
void ClockDisplay_clearBuffer();

/**
 * Call this function repeated to write out the image
 */
void ClockDisplay_writeOutImage();

