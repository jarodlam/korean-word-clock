#include "KoreanClockFace.h"

#include <stdint.h>
#include <stddef.h>

KoreanClockFace::KoreanClockFace(LedMatrix &ledMatrix, uint8_t brightness, uint8_t effectIdx)
  : _brightness(brightness), _effectIdx(effectIdx), _ledMatrix(ledMatrix) {
  _clear();
}

void KoreanClockFace::show() {
  LedColor on = { 255, 255, 255, 255 };
  _ledMatrix.clear();
  for (uint8_t x = 0; x < KOREAN_CLOCK_FACE_WIDTH; ++x) {
    for (uint8_t y = 0; y < KOREAN_CLOCK_FACE_HEIGHT; ++y) {
      LedPosition pos{ x, y };
      if (_state[x][y]) {
        _ledMatrix.setCell(pos, on);
      }
    }
  }
  _ledMatrix.show();
}

void KoreanClockFace::update(DateTime &dt) {
  uint8_t hour24 = dt.hour;
  uint8_t hour12 = hour24 % 12;
  uint8_t minute = dt.minute;
  uint8_t minute10 = minute / 10;  // integer division, rounds toward zero
  uint8_t minute1 = minute % 10;

  _clear();

  // Midnight and midday
  if (hour24 == 0 && minute < 5) {
    _setWord(WORD_MIDNIGHT);
    return;
  }
  if (hour24 == 12 && minute < 5) {
    _setWord(WORD_MIDDAY);
  }

  // Hour
  switch (hour12) {
    case 0: _setWord(WORD_HOUR_12); break;
    case 1: _setWord(WORD_HOUR_1); break;
    case 2: _setWord(WORD_HOUR_2); break;
    case 3: _setWord(WORD_HOUR_3); break;
    case 4: _setWord(WORD_HOUR_4); break;
    case 5: _setWord(WORD_HOUR_5); break;
    case 6: _setWord(WORD_HOUR_6); break;
    case 7: _setWord(WORD_HOUR_7); break;
    case 8: _setWord(WORD_HOUR_8); break;
    case 9: _setWord(WORD_HOUR_9); break;
    case 10: _setWord(WORD_HOUR_10); break;
    case 11: _setWord(WORD_HOUR_11); break;
  }

  // Minute special cases
  if (minute < 5) {
    // On the hour, no minutes
    return;
  }
  if (minute >= 15 && minute < 20) {
    // Special arrangement at 15 past
    _setWord(WORD_MIN_15);
    return;
  }

  // Minute tens digit
  switch (minute10) {
    case 1: _setWord(WORD_MIN_10); break;
    case 2: _setWord(WORD_MIN_20); break;
    case 3: _setWord(WORD_MIN_30); break;
    case 4: _setWord(WORD_MIN_40); break;
    case 5: _setWord(WORD_MIN_50); break;
  }

  // Minute ones digit (just 5)
  if (minute1 >= 5) {
    _setWord(WORD_MIN_5);
  }
}

uint8_t KoreanClockFace::setBrightness(uint8_t brightness) {
  _ledMatrix.setBrightness(brightness);
  return _brightness;
}

void KoreanClockFace::_clear() {
  for (int x = 0; x < KOREAN_CLOCK_FACE_WIDTH; ++x) {
    for (int y = 0; y < KOREAN_CLOCK_FACE_HEIGHT; ++y) {
      _state[x][y] = false;
    }
  }
}

// For testing only
bool *KoreanClockFace::getState() {
  return (bool *)_state;
}
