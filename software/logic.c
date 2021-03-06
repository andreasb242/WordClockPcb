/**
 * Main Project Logic
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#include "logic.h"
#include "lib/usb-cdc.h"
#include "lib/timer.h"
#include "lib/hardware.h"
#include "ClockDisplay.h"
#include "TextDisplay.h"
#include "lib/PFC8563.h"
#include "lib/dataflash.h"
#include "hardware.h"
#include "sw-version.h"

#ifdef HWREF1
#define SETUP_LED P1_7
#else
uint8_t SETUP_LED = 0;
#endif

/**
 * Current Hour
 */
uint8_t clockHour = 0;

/**
 * Current Minute
 */
uint8_t clockMin = 0;

/**
 * Additional 4 LEDs enabled, set clock also supports then 1 minute steps
 */
uint8_t g_additionalLedsEnabled = 1;

/**
 * Current display Buffer (no double buffering)
 */
extern uint16_t m_displayBuffer[9];

/**
 * Touch Samples
 */
#define SAMPLE_TIMES (0x05)

/**
 * Touch value
 */
#define TOUCH_THREASHOLD 2200

/**
 * Read Touch Key
 *
 * @param channel Touch Channel
 * @return Touch Value
 */
uint16_t ReadTouchKey(uint8_t channel) {
	__xdata uint8_t sampleCounter = 5;
	__xdata uint16_t sum = 0;
	__xdata uint16_t overTime;

	TKEY_CTRL |= bTKC_2MS;

	// Select Channel
	TKEY_CTRL = (TKEY_CTRL & 0XF8) | channel;

	// Sample n-times
	while (sampleCounter--) {

		overTime = 0;
		while ((TKEY_CTRL & bTKC_IF) == 0) {
			if (++overTime == 0) {
				return 0;
			}
		}
		sum += TKEY_DAT;
	}

	return sum / SAMPLE_TIMES;
}

/**
 * Initialize Hardware
 */
void logicInit() {
	uint8_t i;

#ifdef HWREF1
	// Setup LED
	// P1_6
	P1_MOD_OC &= ~(1 << 7);
#endif
	SETUP_LED = 0;

	// Setup Touch
	// P1.4
	P1_DIR_PU &= ~(1 << 4);
	P1_MOD_OC &= ~(1 << 4);
	// P1.5
	P1_DIR_PU &= ~(1 << 5);
	P1_MOD_OC &= ~(1 << 5);

	ClockDisplay_init();

	TextDisplay_clock(0, 0);
	delay_ms(1000);
	TextDisplay_clock(1, 0);

	// Dummy Read
	ReadTouchKey(0x03);
	ReadTouchKey(0x04);

	ReadDataFlash(0, 1, &g_additionalLedsEnabled);
}

/**
 * Called from the main loop
 */
void logicLoop() {
	static uint8_t currentState = 0;
	static uint32_t nextTimeStep = 0;
	static uint8_t buttonHourPressed = 0;
	static uint8_t buttonMinPressed = 0;
	static uint32_t lastTimeSync = 5000;
	uint8_t errorCode;
	uint16_t touchValue;
	RtcDateTime time;

	// Read time about every second from RTC
	if (lastTimeSync != g_Timer && currentState != 3) {
		lastTimeSync = g_Timer;

		errorCode = PCF8563_read(&time);
		if (errorCode) {
			// ERROR
			TextDisplay_fehler(errorCode);
		} else {
			clockHour = time.hour;
			clockMin = time.minute;

			TextDisplay_clock(clockHour, clockMin);
		}
	}

	if (currentState == 0) {
		// Minute
		touchValue = ReadTouchKey(0x03);
		if (touchValue > TOUCH_THREASHOLD) {
			return;
		}

		// Hour
		touchValue = ReadTouchKey(0x04);
		if (touchValue > TOUCH_THREASHOLD) {
			return;
		}

		currentState = 1;
		nextTimeStep = g_Timer + 3;
	} else if (currentState == 1) {
		// Minute
		touchValue = ReadTouchKey(0x03);
		if (touchValue > TOUCH_THREASHOLD) {
			currentState = 0;
			return;
		}

		// Hour
		touchValue = ReadTouchKey(0x04);
		if (touchValue > TOUCH_THREASHOLD) {
			currentState = 0;
			return;
		}

		if (nextTimeStep == g_Timer) {
			currentState = 2;
			SETUP_LED = 1;
		}
	} else if (currentState == 2) {
		// Wait for Button Release

		// Minute
		touchValue = ReadTouchKey(0x03);
		if (touchValue < TOUCH_THREASHOLD) {
			// still pressed
			return;
		}

		// Hour
		touchValue = ReadTouchKey(0x04);
		if (touchValue < TOUCH_THREASHOLD) {
			// still pressed
			return;
		}

		currentState = 3;
		nextTimeStep = g_Timer + 60;

	} else if (currentState == 3) {
		// Blink Setup Time
		SETUP_LED = g_Timer & 0x01;

		// Minute
		touchValue = ReadTouchKey(0x03);
		if (touchValue < TOUCH_THREASHOLD) {
			nextTimeStep = g_Timer + 60;

			if (!buttonMinPressed) {
				buttonMinPressed = 1;

				clockMin += 5;
				clockMin = (clockMin / 5) * 5;
				if (clockMin > 59) {
					clockMin = 0;
				}
				TextDisplay_clock(clockHour, clockMin);

				time.second = 0;
				time.minute = clockMin;
				time.hour = clockHour;
				PCF8563_write(&time);

				delay_ms(50);
			} else {
				buttonMinPressed = 0;
				delay_ms(50);
			}
		}

		// Hour
		touchValue = ReadTouchKey(0x04);
		if (touchValue < TOUCH_THREASHOLD) {
			nextTimeStep = g_Timer + 60;

			if (!buttonHourPressed) {
				buttonHourPressed = 1;

				clockHour++;
				if (clockHour > 23) {
					clockHour = 0;
				}
				TextDisplay_clock(clockHour, clockMin);

				time.second = 0;
				time.minute = clockMin;
				time.hour = clockHour;
				PCF8563_write(&time);

				delay_ms(50);
			}
		} else {
			buttonHourPressed = 0;
			delay_ms(50);
		}

		if (nextTimeStep == g_Timer) {
			currentState = 0;
			SETUP_LED = 0;
		}
	}
}

/**
 * Called for each received char
 *
 * @param c Received char
 */
void logicCharReceived(char c) {
	static char receiverCommand = 0;
	static char receiverValue = 0;
	uint8_t i, x;
	uint16_t row;
	RtcDateTime time;

	if (receiverCommand == 0) {
		if (c == 'l') {
			g_additionalLedsEnabled = !g_additionalLedsEnabled;
			WriteDataFlash(0, &g_additionalLedsEnabled, 1);
			UsbCdc_puts("Additional LEDs ");

			if (g_additionalLedsEnabled) {
				UsbCdc_puts("enabled\r\n");
			} else {
				UsbCdc_puts("disabled\r\n");
			}
			return;
		} else if (c == 'm') {
			UsbCdc_puts("Set MIN\r\n");
		} else if (c == 'h') {
			UsbCdc_puts("Set HOUR\r\n");
		} else if (c == 'd') {
			UsbCdc_puts("DUMP\r\n");

			for (i = 0; i < 9; i++) {
				UsbCdc_puti(i);
				UsbCdc_puts(": ");

				row = m_displayBuffer[i];

				for (x = 11; x > 0; x--) {
					if (row & (1 << (x - 1))) {
						UsbCdc_putc('x');
					} else {
						UsbCdc_putc(' ');
					}
				}

				UsbCdc_puts("\r\n");
			}

			return;
		} else if (c == 't') {
			// Touch Test / Readout

			UsbCdc_puts("Minute ");
			row = ReadTouchKey(0x03);
			UsbCdc_puti16(row);
			UsbCdc_puts("\r\n");

			UsbCdc_puts("Hour ");
			row = ReadTouchKey(0x04);
			UsbCdc_puti16(row);
			UsbCdc_puts("\r\n");

			return;
		} else if (c == 'i') {
			// Touch Test / Readout

			UsbCdc_puts("Version: ");
			UsbCdc_puts(GIT_VERSION_SHA_SHORT "\r\n");
			UsbCdc_puts(GIT_VERSION_DATE "\r\n");

			return;
		} else if (c == 'a') {
			// Touch Test / Readout

			UsbCdc_puts("Var Time: ");
			if (clockHour < 10) {
				UsbCdc_putc('0');
			}
			UsbCdc_puti(clockHour);
			UsbCdc_putc(':');
			if (clockMin < 10) {
				UsbCdc_putc('0');
			}
			UsbCdc_puti(clockMin);
			UsbCdc_puts("\r\n");

			return;
		} else if (c == 'z') {
			x = PCF8563_read(&time);
			if (x == 0) {
				UsbCdc_puts("Time ");
				if (time.hour < 10) {
					UsbCdc_putc('0');
				}

				UsbCdc_puti(time.hour);
				UsbCdc_putc(':');

				if (time.minute < 10) {
					UsbCdc_putc('0');
				}
				UsbCdc_puti(time.minute);
				UsbCdc_putc(':');

				if (time.second < 10) {
					UsbCdc_putc('0');
				}
				UsbCdc_puti(time.second);
				UsbCdc_puts("\r\n");

			} else {
				UsbCdc_puts("Could not read RTC ");
				UsbCdc_puti(x);
				UsbCdc_puts("\r\n");
			}

			return;

		} else if (c == 'b') {
			UsbCdc_puts("Boot to Bootloader");
			logicPowerDown();

			delay_ms(1000);

			jumpToBootloader();
			return;
		} else {
			return;
		}

		receiverCommand = c;
		receiverValue = 0;
	} else {
		if (c == '\r' || c == '\n') {

			if (receiverCommand == 'h' || receiverCommand == 'm') {
				if (receiverCommand == 'h') {
					clockHour = receiverValue;
				} else if (receiverCommand == 'm') {
					clockMin = receiverValue;
				}

				TextDisplay_clock(clockHour, clockMin);

				UsbCdc_puts("Set Time:\r\n");
				UsbCdc_puts("H:");
				UsbCdc_puti(clockHour);
				UsbCdc_puts("\r\n");
				UsbCdc_puts("M:");
				UsbCdc_puti(clockMin);
				UsbCdc_puts("\r\n");

				time.second = 0;
				time.minute = clockMin;
				time.hour = clockHour;

				PCF8563_write(&time);
			}

			receiverCommand = 0;
			receiverValue = 0;

		} else if (c >= '0' && c <= '9') {
			receiverValue *= 10;
			receiverValue += c - '0';
		}
	}
}

/**
 * Called before device gets powered down by USB
 */
void logicPowerDown() {
	TextDisplay_clearTmpBuffer();
	TextDisplay_flushData();
	delay_ms(100);
}

