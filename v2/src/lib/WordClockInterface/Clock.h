#pragma once

#include <stdint.h>

struct DateTime {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint16_t year;
};

class Clock {
public:
  virtual DateTime readDateTime() = 0;         // pure virtual
  virtual void setDateTime(DateTime &dt) = 0;  // pure virtual
};
