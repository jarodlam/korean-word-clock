#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/**
 * Controls an RGBW SK6812 matrix.
 * LEDs must be arranged like this:
 *
 *  --<--┐
 * ┌--->-┘
 * └--<--┐
 *  0-->-┘
 */
class LEDMatrix {
public:
  LEDMatrix(uint8_t width, uint8_t height, uint8_t ledsPerCell, int16_t pin);
  void begin();
  void show();
  void clear();
  void setBrightness(uint8_t);

  /**
   * Set LEDs for a cell at a specific coordinate.
   * Coordinates are 0-indexed with (0,0) at bottom-left.
   */
  void setCell(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

private:
  uint8_t width;
  uint8_t height;
  uint8_t ledsPerCell;
  Adafruit_NeoPixel* neopixel;

  uint16_t numLeds();
};
