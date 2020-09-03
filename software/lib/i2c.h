/**
 * RTC Library
 *
 * Andreas Butti, (c) 2020
 * License: MIT
 */

#pragma once

#include "inc.h"

/**
 * Initialize I2C
 */
void I2C_init();

/**
 * Start I2C Communication
 */
void I2C_start();

/**
 * Stop I2C Communication
 */
void I2C_stop();

/**
 * Write byte to I2C Device
 *
 * @param byte Byte to write
 * @return ACK / NACK
 */
uint8_t I2C_Write(uint8_t byte);

/**
 * I2C Read Byte
 *
 * @return Read byte
 */
uint8_t I2C_read();

/**
 * Stop / Start
 */
void I2C_repeatStart();

/**
 * Send the Non-acknoledgement(NACK) bit the i2c bus
 */
void I2C_sendNackBit();

