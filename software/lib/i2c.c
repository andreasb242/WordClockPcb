/**
 * Software I2C Master
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#include "i2c.h"
#include "../hardware.h"

#ifdef HWREF1
#define SDA_BUS P3_1
#define SCL_BUS P3_0
#else
#define SDA_BUS P1_1
#define SCL_BUS P1_7
#endif

/**
 * Delay for I2c
 */
#define I2C_DELAY 50

/**
 * Initialize I2C
 */
void I2C_init() {
	SDA_BUS = 1;
	SCL_BUS = 1;
}

/**
 * I2C Delay function
 * @param d Delay
 */
void I2C_delay(uint16_t d) {
	uint16_t i;
	for (i = 0; i < d; i++)
		;
}

/**
 * Start I2C Communication
 */
void I2C_start() {
	SDA_BUS = 1;
	SCL_BUS = 1;
	I2C_delay(I2C_DELAY);

	SDA_BUS = 0;
	I2C_delay(I2C_DELAY);
}

/**
 * Stop I2C Communication
 */
void I2C_stop() {
	SCL_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SCL_BUS = 1;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 1;
	I2C_delay(I2C_DELAY);
}

/**
 * Write byte to I2C Device
 *
 * @param byte Byte to write
 * @return ACK=1 / NACK=0
 */
uint8_t I2C_Write(uint8_t byte) {
	unsigned char i;

	for (i = 0; i < 8; i++) {
		SCL_BUS = 0;
		I2C_delay(I2C_DELAY);
		if ((byte << i) & 0x80) {
			SDA_BUS = 1;
		} else {
			SDA_BUS = 0;
		}
		I2C_delay(I2C_DELAY / 2);

		SCL_BUS = 1;
		I2C_delay(I2C_DELAY);
	}

	SCL_BUS = 0;
	SDA_BUS = 0;
	I2C_delay(I2C_DELAY / 2);
	SCL_BUS = 1;
	SDA_BUS = 1;
	I2C_delay(I2C_DELAY);
	return !SDA_BUS;
}

/**
 * I2C Read Byte
 *
 * @return Read byte
 */
uint8_t I2C_read() {
	unsigned char i, d, rxdata = 0;

	for (i = 0; i < 8; i++) {
		SCL_BUS = 0;
		SDA_BUS = 1;
		I2C_delay(I2C_DELAY);

		SCL_BUS = 1;
		I2C_delay(I2C_DELAY / 2);

		d = SDA_BUS;
		rxdata = rxdata | (d << 7 - i);
		I2C_delay(I2C_DELAY);
	}

	return rxdata;
}

/**
 * Stop / Start
 */
void I2C_repeatStart() {
	SCL_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 1;
	I2C_delay(I2C_DELAY / 2);

	SCL_BUS = 1;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 0;
	I2C_delay(I2C_DELAY);
}

/**
 * Send the Non-acknoledgement(NACK) bit the i2c bus
 */
void I2C_sendNackBit() {
	SCL_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 1;
	I2C_delay(I2C_DELAY / 2);

	SCL_BUS = 1;
	I2C_delay(I2C_DELAY);
}

/**
 * Send the acknoledgement(ACK) bit the i2c bus
 */
void I2C_sendAckBit() {
	SCL_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SDA_BUS = 0;
	I2C_delay(I2C_DELAY / 2);

	SCL_BUS = 1;
	I2C_delay(I2C_DELAY);
}

