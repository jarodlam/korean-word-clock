#pragma once

#include <stdint.h>

/*
 * Represents a single cell (which can be made up of one or more LEDs).
 * Coordinates are 0-indexed with (0,0) at bottom-left.
 */
struct LedPosition {
  uint8_t x;
  uint8_t y;
};

struct LedColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

/**
 * Interface for an RGBW LED matrix.
 */
class LedMatrix {
public:
  /**
   * Initialise the hardware.
   */
  virtual void begin();

  /**
   * Write the configuration to the LED matrix.
   */
  virtual void show();

  /**
   * Set all LEDs to off.
   */
  virtual void clear();

  /**
   * Set global brightness for the LED strip.
   */
  virtual void setBrightness(uint8_t brightness);

  /**
   * Set LEDs for a cell at a specific coordinate.
   */
  virtual void setCell(LedPosition &pos, LedColor &color);
};
