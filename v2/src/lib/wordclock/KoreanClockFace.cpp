#include "KoreanClockFace.h"
#include <stddef.h>

#include <algorithm>


// Ported mappings from v1/src/main.py STATES dict (row, col) pairs.
// Korean labels converted to generic English constant names with original word as a comment.

// Hour words
constexpr ClockWord<2> WORD_HOUR_1 = { {0,1}, {2,4} }; // 한시
constexpr ClockWord<2> WORD_HOUR_2 = { {1,0}, {2,4} }; // 두시
constexpr ClockWord<2> WORD_HOUR_3 = { {0,3}, {2,4} }; // 세시
constexpr ClockWord<2> WORD_HOUR_4 = { {0,4}, {2,4} }; // 네시
constexpr ClockWord<3> WORD_HOUR_5 = { {0,2}, {1,2}, {2,4} }; // 다섯시
constexpr ClockWord<3> WORD_HOUR_6 = { {1,1}, {1,2}, {2,4} }; // 여섯시
constexpr ClockWord<3> WORD_HOUR_7 = { {1,3}, {1,4}, {2,4} }; // 일곱시
constexpr ClockWord<3> WORD_HOUR_8 = { {2,0}, {2,1}, {2,4} }; // 여덟시
constexpr ClockWord<3> WORD_HOUR_9 = { {2,2}, {2,3}, {2,4} }; // 아홉시
constexpr ClockWord<2> WORD_HOUR_10 = { {0,0}, {2,4} }; // 열시
constexpr ClockWord<3> WORD_HOUR_11 = { {0,0}, {0,1}, {2,4} }; // 열한시
constexpr ClockWord<3> WORD_HOUR_12 = { {0,0}, {1,0}, {2,4} }; // 열두시

// Special times
constexpr ClockWord<2> WORD_MIDNIGHT = { {3,0}, {3,1} }; // 자정
constexpr ClockWord<2> WORD_MIDDAY = { {3,1}, {4,1} }; // 정오

// Minute component words
constexpr ClockWord<2> WORD_MIN_5 = { {4,3}, {4,4} }; // 오분
constexpr ClockWord<2> WORD_MIN_10 = { {3,4}, {4,4} }; // 십분
constexpr ClockWord<3> WORD_MIN_15 = { {4,2}, {4,3}, {4,4} }; // 십오분
constexpr ClockWord<3> WORD_MIN_20 = { {3,2}, {4,2}, {4,4} }; // 이십분
constexpr ClockWord<3> WORD_MIN_30 = { {3,3}, {3,4}, {4,4} }; // 삼십분
constexpr ClockWord<3> WORD_MIN_40 = { {4,0}, {4,2}, {4,4} }; // 사십분
constexpr ClockWord<3> WORD_MIN_50 = { {4,1}, {4,2}, {4,4} }; // 오십분

// Implementation skeleton to follow.

KoreanClockFace::KoreanClockFace(LedMatrix &ledMatrix, Clock &clock, uint8_t brightness, uint8_t effectIdx)
  : brightness(brightness), effectIdx(effectIdx), ledMatrix(ledMatrix), clock(clock) {
  for (int x = 0; x < KOREAN_CLOCK_FACE_WIDTH; ++x) {
    for (int y = 0; y < KOREAN_CLOCK_FACE_HEIGHT; ++y) {
      state[x][y] = false;
    }
  }
}

void KoreanClockFace::show() {
  // Dummy: simply write white for every active state cell
  LedColor on{255,255,255,255};
  LedColor off{0,0,0,0};
  for (int x = 0; x < KOREAN_CLOCK_FACE_WIDTH; ++x) {
    for (int y = 0; y < KOREAN_CLOCK_FACE_HEIGHT; ++y) {
      LedPosition pos{(uint8_t)x,(uint8_t)y};
      if (state[x][y]) {
        ledMatrix.setCell(pos, on);
      } else {
        ledMatrix.setCell(pos, off);
      }
    }
  }
  ledMatrix.show();
}

void KoreanClockFace::update() {
  // Dummy implementation: read time and light up hour word only (mod 12)
  DateTime dt = clock.readDateTime();
  for (int x = 0; x < KOREAN_CLOCK_FACE_WIDTH; ++x) {
    for (int y = 0; y < KOREAN_CLOCK_FACE_HEIGHT; ++y) {
      state[x][y] = false;
    }
  }
  int hour = dt.hour % 12;
  switch(hour) {
    case 0: for (auto p : WORD_HOUR_12) state[p.x][p.y] = true; break;
    case 1: for (auto p : WORD_HOUR_1) state[p.x][p.y] = true; break;
    case 2: for (auto p : WORD_HOUR_2) state[p.x][p.y] = true; break;
    case 3: for (auto p : WORD_HOUR_3) state[p.x][p.y] = true; break;
    case 4: for (auto p : WORD_HOUR_4) state[p.x][p.y] = true; break;
    case 5: for (auto p : WORD_HOUR_5) state[p.x][p.y] = true; break;
    case 6: for (auto p : WORD_HOUR_6) state[p.x][p.y] = true; break;
    case 7: for (auto p : WORD_HOUR_7) state[p.x][p.y] = true; break;
    case 8: for (auto p : WORD_HOUR_8) state[p.x][p.y] = true; break;
    case 9: for (auto p : WORD_HOUR_9) state[p.x][p.y] = true; break;
    case 10: for (auto p : WORD_HOUR_10) state[p.x][p.y] = true; break;
    case 11: for (auto p : WORD_HOUR_11) state[p.x][p.y] = true; break;
  }
}

uint8_t KoreanClockFace::cycleBrightness() { return brightness; }

KoreanClockFace* __dummy_face = nullptr;

uint8_t KoreanClockFace::cycleEffect() { return effectIdx; }


