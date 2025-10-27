#include <unity.h>
#include <stdint.h>
#include <KoreanClockFace.h>
#include <LedMatrix.h>

class FakeLedMatrix : public LedMatrix {};

template<size_t N>
void assertWordSet(const ClockWord<N>& word, bool* rawState) {
  // rawState is a flattened bool[KOREAN_CLOCK_FACE_WIDTH][KOREAN_CLOCK_FACE_HEIGHT]
  for (auto pos : word) {
    size_t idx = pos.x * KOREAN_CLOCK_FACE_HEIGHT + pos.y;
    TEST_ASSERT_TRUE_MESSAGE(rawState[idx], "Expected cell of word to be true");
  }
}

void test_update_hour_only() {
  FakeLedMatrix matrix;
  KoreanClockFace face(matrix);
  // 03:02
  DateTime dt = {
    .second = 0,
    .minute = 2,
    .hour = 3,
  };
  face.update(dt);
  bool* state = face.getState();
  assertWordSet(WORD_HOUR_3, state);
  // Ensure a minute word not set: pick one cell from WORD_MIN_10
  auto pos = WORD_MIN_10[0];
  size_t idx = pos.x * KOREAN_CLOCK_FACE_HEIGHT + pos.y;
  TEST_ASSERT_FALSE(state[idx]);
}

void test_update_quarter_past() {
  FakeLedMatrix matrix;
  KoreanClockFace face(matrix);
  // 10:17
  DateTime dt = {
    .second = 0,
    .minute = 17,
    .hour = 10,
  };
  face.update(dt);
  bool* state = face.getState();
  assertWordSet(WORD_HOUR_10, state);
  assertWordSet(WORD_MIN_15, state);
}

void test_update_twenty_five() {
  FakeLedMatrix matrix;
  KoreanClockFace face(matrix);
  // 20:27
  DateTime dt = {
    .second = 0,
    .minute = 27,
    .hour = 20,
  };
  face.update(dt);
  bool* state = face.getState();
  assertWordSet(WORD_HOUR_8, state);
  assertWordSet(WORD_MIN_20, state);
  assertWordSet(WORD_MIN_5, state);
}

void test_update_midnight() {
  FakeLedMatrix matrix;
  KoreanClockFace face(matrix);
  // 00:03
  DateTime dt = {
    .second = 0,
    .minute = 3,
    .hour = 0,
  };
  face.update(dt);
  bool* state = face.getState();
  assertWordSet(WORD_MIDNIGHT, state);
}

void test_update_midday() {
  FakeLedMatrix matrix;
  KoreanClockFace face(matrix);
  // 12:02
  DateTime dt = {
    .second = 0,
    .minute = 2,
    .hour = 12,
  };
  face.update(dt);
  bool* state = face.getState();
  assertWordSet(WORD_MIDDAY, state);
}

void test_KoreanClockFace() {
  RUN_TEST(test_update_hour_only);
  RUN_TEST(test_update_quarter_past);
  RUN_TEST(test_update_twenty_five);
  RUN_TEST(test_update_midnight);
  RUN_TEST(test_update_midday);
}
