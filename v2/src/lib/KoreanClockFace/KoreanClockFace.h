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
template<size_t N>
using ClockWord = LedPosition[N];

/*
열 한 다 세 네
두 여 섯 일 곱
여 덟 아 홉 시
자 정 이 삼 십
사 오 십 오 분
*/
constexpr ClockWord<2> WORD_HOUR_1 = { { 1, 4 }, { 4, 2 } };             // 한시
constexpr ClockWord<2> WORD_HOUR_2 = { { 0, 3 }, { 4, 2 } };             // 두시
constexpr ClockWord<2> WORD_HOUR_3 = { { 3, 4 }, { 4, 2 } };             // 세시
constexpr ClockWord<2> WORD_HOUR_4 = { { 4, 4 }, { 4, 2 } };             // 네시
constexpr ClockWord<3> WORD_HOUR_5 = { { 2, 4 }, { 2, 3 }, { 4, 2 } };   // 다섯시
constexpr ClockWord<3> WORD_HOUR_6 = { { 1, 3 }, { 2, 3 }, { 4, 2 } };   // 여섯시
constexpr ClockWord<3> WORD_HOUR_7 = { { 3, 3 }, { 4, 3 }, { 4, 2 } };   // 일곱시
constexpr ClockWord<3> WORD_HOUR_8 = { { 0, 2 }, { 1, 2 }, { 4, 2 } };   // 여덟시
constexpr ClockWord<3> WORD_HOUR_9 = { { 2, 2 }, { 3, 2 }, { 4, 2 } };   // 아홉시
constexpr ClockWord<2> WORD_HOUR_10 = { { 0, 4 }, { 4, 2 } };            // 열시
constexpr ClockWord<3> WORD_HOUR_11 = { { 0, 4 }, { 1, 4 }, { 4, 2 } };  // 열한시
constexpr ClockWord<3> WORD_HOUR_12 = { { 0, 4 }, { 0, 3 }, { 4, 2 } };  // 열두시
constexpr ClockWord<2> WORD_MIN_5 = { { 3, 0 }, { 4, 0 } };              // 오분
constexpr ClockWord<2> WORD_MIN_10 = { { 4, 1 }, { 4, 0 } };             // 십분
constexpr ClockWord<3> WORD_MIN_15 = { { 2, 0 }, { 3, 0 }, { 4, 0 } };   // 십오분
constexpr ClockWord<3> WORD_MIN_20 = { { 2, 1 }, { 2, 0 }, { 4, 0 } };   // 이십분
constexpr ClockWord<3> WORD_MIN_30 = { { 3, 1 }, { 4, 1 }, { 4, 0 } };   // 삼십분
constexpr ClockWord<3> WORD_MIN_40 = { { 0, 0 }, { 2, 0 }, { 4, 0 } };   // 사십분
constexpr ClockWord<3> WORD_MIN_50 = { { 1, 0 }, { 2, 0 }, { 4, 0 } };   // 오십분
constexpr ClockWord<2> WORD_MIDNIGHT = { { 0, 1 }, { 1, 1 } };           // 자정
constexpr ClockWord<2> WORD_MIDDAY = { { 1, 1 }, { 1, 0 } };             // 정오

/**
 * Controller for the Korean clock face.
 */
class KoreanClockFace : public ClockFace {
public:
  KoreanClockFace(LedMatrix &ledMatrix, uint8_t brightness = 255, uint8_t effectIdx = 0);

  void show();
  void update(DateTime &dt);
  uint8_t setBrightness(uint8_t brightness);

  /* 
   * For testing only.
   */
  bool *getState();

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
  template<size_t N>
  void _setWord(const ClockWord<N> &word) {
    for (auto pos : word) {
      _state[pos.x][pos.y] = true;
    }
  }
};
