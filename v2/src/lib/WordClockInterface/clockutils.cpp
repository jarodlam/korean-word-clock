#include "clockutils.h"

uint8_t cycleBrightness(uint8_t currBrightness) {
  uint8_t currStep = (currBrightness + 1) / BRIGHTNESS_STEP_SIZE;
  currStep = (currStep + 1) % BRIGHTNESS_NUM_STEPS;
  if (currStep == 0) {
    currStep = 1;
  }
  return ((uint16_t)8 * currStep) - 1;
}

void timePlus(DateTime &dt) {
}

void timeMinus(DateTime &dt) {
}
