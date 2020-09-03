/**
 * Timer file controls Timer0
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#include "PFC8563.h"
#include "i2c.h"
#include "hardware.h"

/**
 * I2C Address
 */
#define PCF8563address (0x51 << 1) // 0xA2

/**
 * Initialize RTC
 */
void PCF8563_init() {
	I2C_init();
}

/**
 * BCD to Decimal
 *
 * @param value BCD
 * @return Decimal
 */
uint8_t bcdToDec(uint8_t value) {
	return ((value / 16) * 10 + value % 16);
}

/**
 * Decimal to BCD
 *
 * @param value Decimal
 * @return BCD
 */
uint8_t decToBcd(uint8_t value) {
	return (value / 10 * 16 + value % 10);
}

/**
 * Connect I2C Slave
 *
 * @param addr Address
 * @return ACK=1 / NACK=0
 */
uint8_t PCF8563_connect(uint8_t addr) {
	I2C_start();

	return I2C_Write(addr);
}

/**
 * Set Date Time
 *
 * @param value Date & Time
 * @return 0 on success, else error code
 */
uint8_t PCF8563_write(RtcDateTime* value) {
	// write address
	if (!PCF8563_connect(0xA2)) {
		return 1;
	}

	if (!I2C_Write(0x02)) {
		return 2;
	}

	if (!I2C_Write(decToBcd(value->second))) {
		return 3;
	}

	if (!I2C_Write(decToBcd(value->minute))) {
		return 4;
	}

	if (!I2C_Write(decToBcd(value->hour))) {
		return 5;
	}

	if (!I2C_Write(decToBcd(0))) {
		return 6;
	}

	if (!I2C_Write(decToBcd(0))) {
		return 7;
	}

	if (!I2C_Write(decToBcd(0))) {
		return 8;
	}

	if (!I2C_Write(decToBcd(0))) {
		return 9;
	}

	I2C_stop();

	return 0;
}





/**
 * RTC Read Date & Time
 *
 * @param val Value output
 * @return 0 on success, else error code
 */
uint8_t PCF8563_read(RtcDateTime *value) {
    I2C_start();

	if (!I2C_Write(PCF8563address | 0)) {
		return 1;
	}

	if (!I2C_Write(0x02)) {
		return 2;
	}

    I2C_repeatStart();

	if (!I2C_Write(PCF8563address | 1)) {
		return 3;
	}

	value->second = bcdToDec(I2C_read() & 0b01111111);
	I2C_sendAckBit();

	value->minute = bcdToDec(I2C_read() & 0b01111111);
	I2C_sendAckBit();

	value->hour = bcdToDec(I2C_read() & 0b00111111);

	I2C_sendNackBit();
	I2C_stop();

	return 0;
}




