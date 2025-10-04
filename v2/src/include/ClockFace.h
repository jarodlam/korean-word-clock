#pragma once

#include <stdint.h>

/**
 * Interface for controlling the word clock face.
 * A ClockFace translates the current time and settings into LED commands on the device.
 */
class ClockFace {
  /**
   * Write the current state to the LEDs.
   */
  virtual void show();

  /**
   * Read the time and other settings and figure out which LEDs to turn on.
   */
  virtual void update();

  /**
   * Cycle through brightness settings.
   * Returns the resulting brightness value.
   */
  virtual uint8_t cycleBrightness();

  /**
   * Cycle through effect settings.
   * Returns the resulting effect index.
   */
  virtual uint8_t cycleEffect();
};
