#pragma once

#include <stdint.h>

#include <Clock.h>

/**
 * Interface for controlling the word clock face.
 * A ClockFace translates the current time and settings into LED commands on the device.
 */
class ClockFace {
  /**
   * Initialise the clock face with default settings.
   */
  virtual void begin(uint8_t brightness, uint8_t effectIdx);

  /**
   * Write the current state to the LEDs.
   * Takes into account internal state such as brightness and effects, if applicable.
   */
  virtual void show();

  /**
   * Update the clock face with the given time.
   */
  virtual void update(DateTime& dt);

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
