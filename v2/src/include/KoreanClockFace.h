#pragma once

#include "Clock.h"
#include "ClockFace.h"
#include "LedMatrix.h"

#define KOREAN_CLOCK_FACE_WIDTH 5;
#define KOREAN_CLOCK_FACE_HEIGHT 5;

/**
 * A word comprises one or more cells on the clock face.
 */
template <size_t N>
using ClockWord = LedPosition[N];

/**
 * Controller for the Korean clock face.
 */
class KoreanClockFace : public ClockFace {
public:
  KoreanClockFace(LedMatrix &ledMatrix, Clock &clock, uint8_t brightness = 255, uint8_t effectIdx = 0);

  void show();
  void update();
  uint8_t cycleBrightness();
  uint8_t cycleEffect();

private:
  uint8_t brightness;
  uint8_t effectIdx;
  bool state[KOREAN_CLOCK_FACE_WIDTH][KOREAN_CLOCK_FACE_HEIGHT];

  /**
   * Turns on a word.
   */
  template <size_t N>
  void setWord(const ClockWord<N>& word);
};
