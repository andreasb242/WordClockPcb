/**
 * Disply framebuffer driver
 */

// 00: ESWISTRFÜNF
// 01: ZEHNZWANZIG
// 02: VIERTELAVOR
// 03: NACHTHALBER
// 04: ZWÖLFDREINS
// 05: ELFÜNFZWEIT
// 06: REVIERSECHS
// 07: SIEBENACHTQ
// 08: ZEHNEUNTUHR
#include "TextDisplay.h"


// TODO DEBUG
#include "lib/usb-cdc.h"


//! Current display Buffer (no double buffering)
extern uint16_t m_displayBuffer[9];

//! Temp Display Buffer
uint16_t tmpBuf[9];

void TextDisplay_txtEs() {
	//         00: ESWISTRFÜNF
	tmpBuf[0] |= 0b11000000000;
}

void TextDisplay_txtIst() {
	//         00: ESWISTRFÜNF
	tmpBuf[0] |= 0b00011100000;
}

void TextDisplay_txtFuenf() {
	//         00: ESWISTRFÜNF
	tmpBuf[0] |= 0b00000001111;
}

void TextDisplay_txtZehn() {
	//         01: ZEHNZWANZIG
	tmpBuf[1] |= 0b11110000000;
}

void TextDisplay_txtZwanzig() {
	//         01: ZEHNZWANZIG
	tmpBuf[1] |= 0b00001111111;
}

void TextDisplay_txtViertel() {
	//         02: VIERTELAVOR
	tmpBuf[2] |= 0b11111110000;
}

void TextDisplay_txtNach() {
	//         03: NACHTHALBER
	tmpBuf[3] |= 0b11110000000;
}

void TextDisplay_txtVor() {
	//         02: VIERTELAVOR
	tmpBuf[2] |= 0b00000000111;
}

void TextDisplay_txtHalb() {
	//         03: NACHTHALBER
	tmpBuf[3] |= 0b00000111100;
}

void TextDisplay_txtZwoelfUhr() {
	//         04: ZWÖLFDREINS
	tmpBuf[4] |= 0b11111000000;
}

void TextDisplay_txtUhr() {
	//         08: ZEHNEUNTUHR
	tmpBuf[8] |= 0b00000000111;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

void TextDisplay_txtEinUhr() {
	//         04: ZWÖLFDREINS
	tmpBuf[4] |= 0b00000001110;
}

void TextDisplay_txtS() {
	//         04: ZWÖLFDREINS
	tmpBuf[4] |= 0b00000000001;
}

void TextDisplay_txtZweiUhr() {
	//         05: ELFÜNFZWEIT
	tmpBuf[5] |= 0b00000011110;
}

void TextDisplay_txtDreiUhr() {
	//         05: ELFÜNFZWEIT
	tmpBuf[5] |= 0b00000011110;
}

void TextDisplay_txtVierUhr() {
	//         06: REVIERSECHS
	tmpBuf[6] |= 0b00111100000;
}

void TextDisplay_txtFuenfUhr() {
	//         05: ELFÜNFZWEIT
	tmpBuf[5] |= 0b00111100000;
}

void TextDisplay_txtSechsUhr() {
	//         06: REVIERSECHS
	tmpBuf[6] |= 0b00000011111;
}

void TextDisplay_txtSiebenUhr() {
	//         07: SIEBENACHTQ
	tmpBuf[7] |= 0b11111100000;
}

void TextDisplay_txtAchtUhr() {
	//         07: SIEBENACHTQ
	tmpBuf[7] |= 0b00000011110;
}

void TextDisplay_txtNeunUhr() {
	//         08: ZEHNEUNTUHR
	tmpBuf[8] |= 0b00011110000;
}

void TextDisplay_txtZehnUhr() {
	//         08: ZEHNEUNTUHR
	tmpBuf[8] |= 0b11110000000;
}

void TextDisplay_txtElfUhr() {
	//         05: ELFÜNFZWEIT
	tmpBuf[5] |= 0b11100000000;
}

/**
 * Flush display data
 */
void TextDisplay_flushData() {
	uint8_t i;
	for (i = 0; i < 9; i++) {
		m_displayBuffer[i] = tmpBuf[i];
	}
}

/**
 * Clear the current buffer
 */
void TextDisplay_clearTmpBuffer() {
	uint8_t i;
	for (i = 0; i < 9; i++) {
		tmpBuf[i] = 0;
	}
}

/**
 * Show "Fehler"
 *
 * @param code Error Code
 */
void TextDisplay_fehler(uint8_t code) {
	TextDisplay_clearTmpBuffer();

	//         00: ESWISTRFÜNF
	tmpBuf[0] |= 0b00000001000;
	//         01: ZEHNZWANZIG
	tmpBuf[1] |= 0b01100000000;
	//         02: VIERTELAVOR
	tmpBuf[2] |= 0b00000010000;
	//         06: REVIERSECHS
	tmpBuf[6] |= 0b00001100000;

	tmpBuf[7]  = code << 1;

	TextDisplay_flushData();
}

/**
 * Write out clock value
 *
 * @param hour Hour
 * @param minute Minute
 */
void TextDisplay_clock(uint8_t hour, uint8_t minute) {
	TextDisplay_clearTmpBuffer();

	TextDisplay_txtEs();
	TextDisplay_txtIst();

	if (minute > 24) { // 12:25 => 5 vor halb eins
		hour++;
	}

	hour = hour % 12;

	if (hour == 1 && minute > 24) {
		// Halb ein*s*, virtel vor ein*s*
		// Und 5 vor halb...
		TextDisplay_txtS();
	}

	if (hour == 0) {
		TextDisplay_txtZwoelfUhr();
	} else if (hour == 1) {
		TextDisplay_txtEinUhr();

		if (minute != 0) {
			TextDisplay_txtS();
		}
	} else if (hour == 2) {
		TextDisplay_txtZweiUhr();
	} else if (hour == 3) {
		TextDisplay_txtDreiUhr();
	} else if (hour == 4) {
		TextDisplay_txtVierUhr();
	} else if (hour == 5) {
		TextDisplay_txtFuenfUhr();
	} else if (hour == 6) {
		TextDisplay_txtSechsUhr();
	} else if (hour == 7) {
		TextDisplay_txtSiebenUhr();
	} else if (hour == 8) {
		TextDisplay_txtAchtUhr();
	} else if (hour == 9) {
		TextDisplay_txtNeunUhr();
	} else if (hour == 10) {
		TextDisplay_txtZehnUhr();
	} else if (hour == 11) {
		TextDisplay_txtElfUhr();
	}

	minute = minute / 5;

	if (minute == 0) { // 0 ... 4
		// Uhr
		TextDisplay_txtUhr();
	} else if (minute == 1) { // 5 ... 9
		// 5 nach
		TextDisplay_txtFuenf();
		TextDisplay_txtNach();
	} else if (minute == 2) { // 10 ... 14
		// 10 nach
		TextDisplay_txtZehn();
		TextDisplay_txtNach();
	} else if (minute == 3) { // 15 ... 19
		// Viertel nach
		TextDisplay_txtViertel();
		TextDisplay_txtNach();
	} else if (minute == 4) { // 20 ... 24
		// 20 nach
		TextDisplay_txtZwanzig();
		TextDisplay_txtNach();
	} else if (minute == 5) { // 25 ... 29
		// 5 vor halb
		TextDisplay_txtFuenf();
		TextDisplay_txtVor();
		TextDisplay_txtHalb();
	} else if (minute == 6) { // 30 ... 34
		TextDisplay_txtHalb();
	} else if (minute == 7) { // 35 ... 39
		TextDisplay_txtFuenf();
		TextDisplay_txtNach();
		TextDisplay_txtHalb();
	} else if (minute == 8) { // 40 ... 44
		TextDisplay_txtZwanzig();
		TextDisplay_txtVor();
	} else if (minute == 9) { // 45 ... 49
		TextDisplay_txtViertel();
		TextDisplay_txtVor();
	} else if (minute == 10) { // 50 ... 54
		TextDisplay_txtZehn();
		TextDisplay_txtVor();
	} else if (minute == 11) { // 55 ... 59
		TextDisplay_txtFuenf();
		TextDisplay_txtVor();
	}

	TextDisplay_flushData();
}
