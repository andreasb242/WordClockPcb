/**
 * Timer file controls Timer0
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#include "timer.h"
#include "../ClockDisplay.h"

/**
 * Internal timer
 */
volatile uint32_t g_Timer = 0;

/**
 * Increment each timer interrupt, then increment g_Timer
 */
uint32_t g_TimerSubtime = 0;

/**
 * Timer configuration
 */
#define LH_RELOAD 0xF5

/**
 * Setup Timer
 */
void timerSetup() {
	// set timer 0 mode 1
	TMOD = 0x01;
	TH0 = LH_RELOAD;
	TL0 = 0x00;

	// start timer 0
	TR0 = 1;

	// enable timer 0 interrupt
	ET0 = 1;
}

/**
 * Called from timer interrupt
 */
inline void timer0clock() {
	g_TimerSubtime++;

	if (g_TimerSubtime > 800) {
		g_TimerSubtime = 0;
		g_Timer++;
	}

	TH0 = LH_RELOAD;
	TL0 = 0x00;

	ClockDisplay_writeOutImage();
}
