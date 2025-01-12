#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <string.h>

// Default I2C address
#define RV3028_DEFAULT_ADDRESS 0x52

// I2C registers
#define RV3028_REG_SECONDS 0x00
#define RV3028_REG_STATUS 0x0E
#define RV3028_REG_CONTROL1 0x0F
#define RV3028_REG_EEADDR 0x25
#define RV3028_REG_EEDATA 0x26
#define RV3028_REG_EECMD 0x27

// EEPROM registers
#define RV3028_REG_BACKUP 0x37

// Bit positions in registers
#define RV3028_POS_STATUS_EEBUSY 7
#define RV3028_POS_CONTROL1_EERD 3
#define RV3028_POS_BACKUP_BSM 2
#define RV3028_POS_BACKUP_FEDE 4

// Battery switching modes
#define RV3028_SWITCHOVER_DISABLED 0x00
#define RV3028_DIRECT_SWITCHING_MODE 0x01
#define RV3028_LEVEL_SWITCHING_MODE 0x02

struct DateTime {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

class RV3028 {
public:
  RV3028(uint8_t addr = RV3028_DEFAULT_ADDRESS);

  /**
   * Perform initial setup.
   */
  void begin();

  /**
   * Read the current date and time from the RV3028.
   */
  DateTime readDateTime();

  /**
   * Set a new date and time on the RV3028.
   */
  void setDateTime(DateTime dt);

private:
  /**
   * I2C address.
   */
  uint8_t addr;

  /**
   * Read a single byte from an I2C register.
   */
  uint8_t i2cRead(uint8_t reg);

  /**
   * Read multiple bytes from I2C registers.
   */
  void i2cRead(uint8_t reg, char* buf, size_t len);

  /**
   * Write a single byte to an I2C register.
   */
  void i2cWrite(uint8_t reg, uint8_t value);

  /**
   * Write multiple bytes to I2C registers.
   */
  void i2cWrite(uint8_t reg, char* buf, size_t len);
  
  /**
   * Read a single bit from a register.
   * Used for reading status bits.
   */
  bool i2cReadBit(uint8_t reg, uint8_t bitPos);
  
  /**
   * Write a single bit to a register while keeping all other bits the same.
   * Used for changing config bits.
   */
  void i2cWriteBit(uint8_t reg, uint8_t bitPos, bool bitVal);

  /**
   * Read a byte from the on-board EEPROM.
   */
  uint8_t eepromRead(uint8_t eepromAddr);

  /**
   * Write a byte to the on-board EEPROM.
   */
  void eepromWrite(uint8_t eepromAddr, uint8_t value);

  /**
   * Wait until the EEBusy bit is cleared before continuing an EEPROM read or write.
   */
  void eepromWaitBusy();

  /**
   * Set battery switchover mode.
   * See datasheet section 4.2, page 45.
   *
   * Can supply the following values:
   * - RV3028_SWITCHOVER_DISABLED: Switchover disabled.
   * - RV3028_DIRECT_SWITCHING_MODE: Switchover when V_DD < V_BACKUP. 
       For when V_DD is significantly higher than V_BACKUP (e.g. 5 V).
   * - RV3028_LEVEL_SWITCHING_MODE: Switchover when V_DD < 2.0 V. 
       For when V_DD is around the same as V_BACKUP (e.g. 3.5 V).
   */
  void setBatterySwitchoverMode(uint8_t mode);

  /**
   * Encode an int to a 2-digit binary-coded decimal number.
   * Only decodes two least significant decimal digits up to 99.
   */
  static uint8_t encodeBcd(uint8_t normalInt);

  /**
   * Decode a 2-digit binary-coded decimal number to int.
   */
  static uint8_t decodeBcd(uint8_t bcdInt);
};