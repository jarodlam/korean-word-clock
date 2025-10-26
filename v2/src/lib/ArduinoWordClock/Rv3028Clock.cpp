#include "Rv3028Clock.h"

Rv3028Clock::Rv3028Clock(uint8_t addr)
  : addr(addr) {}

void Rv3028Clock::begin() {
  Wire.begin();
  setBatterySwitchoverMode(RV3028_DIRECT_SWITCHING_MODE);
}

DateTime Rv3028Clock::readDateTime() {
  /*
  Read the following registers:
  0x00: second
  0x01: minute
  0x02: hour
  0x03: weekday
  0x04: day
  0x05: month
  0x06: year
  */
  uint8_t encoded[7] = { 0 };
  i2cRead(RV3028_REG_SECONDS, encoded, 7);

  // Decode each value from BCD to regular int
  uint8_t decoded[7];
  for (int i = 0; i < 7; i++) {
    decoded[i] = decodeBcd(encoded[i]);
  }

  return DateTime{
    decoded[0],
    decoded[1],
    decoded[2],
    decoded[3],
    decoded[4],
    decoded[5],
    decoded[6] + (uint16_t)2000,  // only last two year digits are stored
  };
}

void Rv3028Clock::setDateTime(DateTime &dt) {
  // Encode each value from regular int to BCD
  uint8_t encoded[7] = {
    encodeBcd(dt.second),
    encodeBcd(dt.minute),
    encodeBcd(dt.hour),
    encodeBcd(dt.weekday),
    encodeBcd(dt.day),
    encodeBcd(dt.month),
    encodeBcd(dt.year % 100),
  };

  /*
  Write to the following registers:
  0x00: second
  0x01: minute
  0x02: hour
  0x03: weekday
  0x04: day
  0x05: month
  0x06: year
  */
  i2cWrite(RV3028_REG_SECONDS, encoded, 7);
}

uint8_t Rv3028Clock::i2cRead(uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}

void Rv3028Clock::i2cRead(uint8_t reg, uint8_t *buf, size_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, len);
  for (uint16_t i = 0; i < len; i++) {
    if (Wire.available() <= 0) {
      break;
    }
    buf[i] = Wire.read();
  }
}

void Rv3028Clock::i2cWrite(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void Rv3028Clock::i2cWrite(uint8_t reg, uint8_t *buf, size_t len) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(buf, len);
  Wire.endTransmission();
}

bool Rv3028Clock::i2cReadBit(uint8_t reg, uint8_t bitPos) {
  uint8_t regVal = i2cRead(reg);
  return (regVal >> bitPos) & 1;
}

void Rv3028Clock::i2cWriteBit(uint8_t reg, uint8_t bitPos, bool bitVal) {
  uint8_t regVal = i2cRead(reg);
  if (bitVal) {
    regVal = regVal | (1 << bitPos);
  } else {
    regVal = regVal & ~(1 << bitPos);
  }
  i2cWrite(reg, regVal);
}

uint8_t Rv3028Clock::eepromRead(uint8_t eepromAddr) {
  /*
  Before starting to read a byte in the EEPROM,
  the auto refresh of the registers from the EEPROM has to be disabled
  by writing 1 into the EERD control bit.
  */
  i2cWriteBit(RV3028_REG_CONTROL1, RV3028_POS_CONTROL1_EERD, true);
  eepromWaitBusy();

  /*
  In order to read a single byte from the EEPROM,
  the address to be read is entered in the EEADDR register,
  then the command 00h is written in the EECMD register,
  then the second command 22h is written in the EECMD register
  and the resulting byte can be read from the EEDATA register.
  */
  i2cWrite(RV3028_REG_EEADDR, eepromAddr);
  i2cWrite(RV3028_REG_EECMD, (uint8_t)0x00);
  i2cWrite(RV3028_REG_EECMD, (uint8_t)0x22);
  eepromWaitBusy();
  uint8_t value = i2cRead(RV3028_REG_EEDATA);

  /*
  When the transfer is finished (EEbusy = 0),
  the user can enable again the auto refresh of the registers
  by writing 0 into the EERD bit in the Control 1 register.
  */
  eepromWaitBusy();
  i2cWriteBit(RV3028_REG_CONTROL1, RV3028_POS_CONTROL1_EERD, false);

  return value;
}

void Rv3028Clock::eepromWrite(uint8_t eepromAddr, uint8_t value) {
  /*
  Before starting to change data stored in the EEPROM,
  the auto refresh of the registers from the EEPROM has to be disabled
  by writing 1 into the EERD control bit.
  */
  i2cWriteBit(RV3028_REG_CONTROL1, RV3028_POS_CONTROL1_EERD, true);
  eepromWaitBusy();

  /*
  In order to write a single byte to the EEPROM,
  the address to which the data must be written is entered in the EEADDR
  register and the data to be written is entered in the EEDATA register, then
  the command 00h is written in the EECMD register, then a second command 21h is
  written in the EECMD register to start the EEPROM write.
  */
  i2cWrite(RV3028_REG_EEADDR, eepromAddr);
  i2cWrite(RV3028_REG_EEDATA, value);
  i2cWrite(RV3028_REG_EECMD, (uint8_t)0x00);
  i2cWrite(RV3028_REG_EECMD, (uint8_t)0x21);

  /*
  When the transfer is finished (EEbusy = 0),
  the user can enable again the auto refresh of the registers
  by writing 0 into the EERD bit in the Control 1 register.
  */
  eepromWaitBusy();
  i2cWriteBit(RV3028_REG_CONTROL1, RV3028_POS_CONTROL1_EERD, false);
}

void Rv3028Clock::eepromWaitBusy() {
  /*
  Wait until the EEBUSY bit is cleared
  before continuing an EEPROM read or write operation.
  */
  while (true) {
    bool busy = i2cReadBit(RV3028_REG_STATUS, RV3028_POS_STATUS_EEBUSY);
    if (!busy) {
      break;
    }
  }
}

void Rv3028Clock::setBatterySwitchoverMode(uint8_t mode) {
  // Read the current value of the EEPROM Backup register
  uint8_t value = eepromRead(RV3028_REG_BACKUP);

  // Clear the BSM and FEDE (Fast Edge Detection Enable) bits
  value = value & ~((1 << RV3028_POS_BACKUP_BSM) & (1 << (RV3028_POS_BACKUP_BSM + 1)) & (1 << RV3028_POS_BACKUP_FEDE));

  // Write the BSM bits
  value = value | ((mode & 0x3) << RV3028_POS_BACKUP_BSM);

  // Set FEDE to 1, required for automatic battery switchover
  if (mode != RV3028_SWITCHOVER_DISABLED) {
    value = value | (1 << RV3028_POS_BACKUP_FEDE);
  }

  // Write the updated config to the EEPROM Backup register
  eepromWrite(RV3028_REG_BACKUP, value);
}

uint8_t Rv3028Clock::encodeBcd(uint8_t normalInt) {
  uint8_t lower = normalInt % 10;
  uint8_t upper = (normalInt / 10) % 10;
  return lower + (upper << 4);
}

uint8_t Rv3028Clock::decodeBcd(uint8_t bcdInt) {
  uint8_t lower = bcdInt & 0x0F;
  uint8_t upper = (bcdInt & 0xF0) >> 4;
  return lower + (upper * 10);
}
