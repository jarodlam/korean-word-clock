#pragma once

#include <stddef.h>

#include "Clock.h"
#include "ClockFace.h"
#include "LedMatrix.h"

#define KOREAN_CLOCK_FACE_WIDTH 5
#define KOREAN_CLOCK_FACE_HEIGHT 5

/**
 * A word comprises one or more cells on the clock face.
 */
template <size_t N>
using ClockWord = LedPosition[N];

/*
열 한 다 세 네
두 여 섯 일 곱
여 덟 아 홉 시
자 정 이 삼 십
사 오 십 오 분
*/
constexpr ClockWord<2> WORD_HOUR_1 = { { 0, 1 }, { 2, 4 } };             // 한시
constexpr ClockWord<2> WORD_HOUR_2 = { { 1, 0 }, { 2, 4 } };             // 두시
constexpr ClockWord<2> WORD_HOUR_3 = { { 0, 3 }, { 2, 4 } };             // 세시
constexpr ClockWord<2> WORD_HOUR_4 = { { 0, 4 }, { 2, 4 } };             // 네시
constexpr ClockWord<3> WORD_HOUR_5 = { { 0, 2 }, { 1, 2 }, { 2, 4 } };   // 다섯시
constexpr ClockWord<3> WORD_HOUR_6 = { { 1, 1 }, { 1, 2 }, { 2, 4 } };   // 여섯시
constexpr ClockWord<3> WORD_HOUR_7 = { { 1, 3 }, { 1, 4 }, { 2, 4 } };   // 일곱시
constexpr ClockWord<3> WORD_HOUR_8 = { { 2, 0 }, { 2, 1 }, { 2, 4 } };   // 여덟시
constexpr ClockWord<3> WORD_HOUR_9 = { { 2, 2 }, { 2, 3 }, { 2, 4 } };   // 아홉시
constexpr ClockWord<2> WORD_HOUR_10 = { { 0, 0 }, { 2, 4 } };            // 열시
constexpr ClockWord<3> WORD_HOUR_11 = { { 0, 0 }, { 0, 1 }, { 2, 4 } };  // 열한시
constexpr ClockWord<3> WORD_HOUR_12 = { { 0, 0 }, { 1, 0 }, { 2, 4 } };  // 열두시
constexpr ClockWord<2> WORD_MIN_5 = { { 4, 3 }, { 4, 4 } };              // 오분
constexpr ClockWord<2> WORD_MIN_10 = { { 3, 4 }, { 4, 4 } };             // 십분
constexpr ClockWord<3> WORD_MIN_15 = { { 4, 2 }, { 4, 3 }, { 4, 4 } };   // 십오분
constexpr ClockWord<3> WORD_MIN_20 = { { 3, 2 }, { 4, 2 }, { 4, 4 } };   // 이십분
constexpr ClockWord<3> WORD_MIN_30 = { { 3, 3 }, { 3, 4 }, { 4, 4 } };   // 삼십분
constexpr ClockWord<3> WORD_MIN_40 = { { 4, 0 }, { 4, 2 }, { 4, 4 } };   // 사십분
constexpr ClockWord<3> WORD_MIN_50 = { { 4, 1 }, { 4, 2 }, { 4, 4 } };   // 오십분
constexpr ClockWord<2> WORD_MIDNIGHT = { { 3, 0 }, { 3, 1 } };           // 자정
constexpr ClockWord<2> WORD_MIDDAY = { { 3, 1 }, { 4, 1 } };             // 정오

/**
 * Controller for the Korean clock face.
 */
class KoreanClockFace : public ClockFace {
public:
  KoreanClockFace(LedMatrix &ledMatrix, uint8_t brightness = 255, uint8_t effectIdx = 0);

  void begin(uint8_t brightness, uint8_t effectIdx);
  void show();
  void update(DateTime &dt);
  uint8_t cycleBrightness();
  uint8_t cycleEffect();

  /* 
   * For testing only.
   */
  bool* getState();

private:
  uint8_t _brightness;
  uint8_t _effectIdx;
  bool _state[KOREAN_CLOCK_FACE_WIDTH][KOREAN_CLOCK_FACE_HEIGHT];
  LedMatrix &_ledMatrix;

  /**
   * Sets the entire _state array to false.
   */
  void _clear();

  /**
   * Turns on a word.
   */
  template <size_t N>
  void _setWord(const ClockWord<N>& word) {
    for (auto pos : word) {
      _state[pos.x][pos.y] = true;
    }
  }
};
