#include "clockutils.h"

uint8_t cycleBrightness(uint8_t currBrightness) {
  uint8_t currStep = (currBrightness + 1) / BRIGHTNESS_STEP_SIZE;
  currStep = (currStep + 1) % BRIGHTNESS_NUM_STEPS;
  if (currStep == 0) {
    currStep = 1;
  }
  return (8 * currStep) - 1;
}

static inline void roundDownToFive(DateTime &dt) {
  dt.minute = dt.minute - (dt.minute % 5);
  dt.second = 0;
}

void timePlus(DateTime &dt) {
  roundDownToFive(dt);
  if (dt.minute + 5 >= 60) {
    dt.minute = dt.minute + 5 - 60;
    dt.hour = (dt.hour + 1) % 24;
  } else {
    dt.minute = dt.minute + 5;
  }
}

void timeMinus(DateTime &dt) {
  roundDownToFive(dt);
  if (dt.minute < 5) {
    dt.minute = dt.minute + 60 - 5;
    dt.hour = (dt.hour + 23) % 24;
  } else {
    dt.minute = dt.minute - 5;
  }
}
