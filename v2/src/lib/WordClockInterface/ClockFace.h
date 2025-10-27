#pragma once

#include <stdint.h>
#include "Clock.h"

/**
 * Interface for controlling the word clock face.
 * A ClockFace translates the current time and settings into LED commands on the device.
 */
class ClockFace {
public:
  virtual ~ClockFace() {}
  /**
   * Write the current state to the LEDs.
   * Takes into account internal state such as brightness and effects, if applicable.
   */
  virtual void show() {}

  /**
   * Update the clock face with the given time.
   */
  virtual void update(DateTime& dt) {}

  /**
   * Set the universal brightness of the clock face.
   */
  uint8_t setBrightness(uint8_t brightness);
};
