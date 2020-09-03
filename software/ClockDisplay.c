/**
 * Clock Hardware Control (Shift Register Matrix)
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#include "ClockDisplay.h"
#include "lib/hardware.h"

#define SERCLR P1_6
#define SER P3_2
#define SERCLK P3_3
#define RCLK P3_4

#define _BV(bit) (1 << (bit))

//! Control LED Bitmask
uint8_t m_ControlLeds;

//! Current display row
uint8_t m_currentRow;

//! Current display Buffer (no double buffering)
uint16_t m_displayBuffer[9];

/**
 * Initialize Clock Display
 */
void ClockDisplay_init() {
	m_currentRow = 0;
	m_ControlLeds = 0xf;

	// Shift Register Output to PUSH/PULL
	// P1_6
	P1_MOD_OC &= ~(1 << 6);
	// P3_2
	P3_MOD_OC &= ~(1 << 2);
	// P3_3
	P3_MOD_OC &= ~(1 << 3);
	// P3_4
	P3_MOD_OC &= ~(1 << 4);

	ClockDisplay_clearBuffer();
}

/**
 * Clear the current buffer
 */
void ClockDisplay_clearBuffer() {
	uint8_t i;
	for (i = 0; i < 10; i++) {
		m_displayBuffer[i] = 0;
	}
}

/**
 * Call this function repeated to write out the image
 */
void ClockDisplay_writeOutImage() {
	int8_t i;
	uint8_t row;
	uint16_t rowData;

	// Latch low -> Freeze current output
	RCLK = 0;

	// Reset Shift Register
	SERCLR = 0;
	SERCLR = 1;

	if (m_currentRow < 5) {
		row = m_currentRow;
	} else { // 5 ... 9
		row = m_currentRow + 3;
	}

	// Write Row
	for (i = 0; i < 16; i++) {
		SER = (i == row);

		SERCLK = 0;
		SERCLK = 1;
	}

	// Write Column
	rowData = m_displayBuffer[m_currentRow];
	for (i = 16; i > 0; i--) {
		SER = !(rowData & (1 << (i - 6)));

		SERCLK = 0;
		SERCLK = 1;
	}

	// Latch High -> Show new output
	RCLK = 1;

	m_currentRow++;
	if (m_currentRow >= 9) {
		m_currentRow = 0;
	}
}

