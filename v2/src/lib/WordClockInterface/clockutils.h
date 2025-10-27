#pragma once

#include <stdint.h>
#include "Clock.h"

constexpr uint8_t BRIGHTNESS_NUM_STEPS = 8;
constexpr uint8_t BRIGHTNESS_STEP_SIZE = 255 / BRIGHTNESS_NUM_STEPS;

uint8_t cycleBrightness(uint8_t currBrightness);
void timePlus(DateTime& dt);
void timeMinus(DateTime& dt);
