#pragma once

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "LEDMatrix.h"

/**
 * Controls an RGBW SK6812 matrix.
 * LEDs must be arranged like this:
 *
 *  --<--┐
 * ┌--->-┘
 * └--<--┐
 *  0-->-┘
 */
class Sk6812LedMatrix : public LedMatrix {
public:
  Sk6812LedMatrix(uint8_t width, uint8_t height, uint8_t ledsPerCell, int16_t pin);
  void begin();
  void show();
  void clear();
  void setBrightness(uint8_t brightness);
  void setCell(LedPosition &pos, LedColor &color);

private:
  uint8_t width;
  uint8_t height;
  uint8_t ledsPerCell;
  Adafruit_NeoPixel *neopixel;

  uint16_t numLeds();
};
