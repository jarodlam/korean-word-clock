#include <unity.h>

#include <clockutils.h>

void test_cycle_brightness() {
  // Representative inputs across the range verify expected step outputs
  TEST_ASSERT_EQUAL_UINT8(7, cycleBrightness(0));
  TEST_ASSERT_EQUAL_UINT8(15, cycleBrightness(30));
  TEST_ASSERT_EQUAL_UINT8(15, cycleBrightness(31));
  TEST_ASSERT_EQUAL_UINT8(23, cycleBrightness(62));
  TEST_ASSERT_EQUAL_UINT8(31, cycleBrightness(93));
  TEST_ASSERT_EQUAL_UINT8(39, cycleBrightness(124));
  TEST_ASSERT_EQUAL_UINT8(47, cycleBrightness(155));
  TEST_ASSERT_EQUAL_UINT8(55, cycleBrightness(186));
  TEST_ASSERT_EQUAL_UINT8(7, cycleBrightness(217));
  TEST_ASSERT_EQUAL_UINT8(7, cycleBrightness(246));
  TEST_ASSERT_EQUAL_UINT8(7, cycleBrightness(254));
}

void test_timePlus_round_down_and_add() {
  DateTime dt{ .second = 42, .minute = 3, .hour = 10, .weekday = 1, .day = 1, .month = 1, .year = 2025 };
  timePlus(dt);
  TEST_ASSERT_EQUAL_UINT8(0, dt.second);  // seconds reset
  TEST_ASSERT_EQUAL_UINT8(5, dt.minute);  // 3 -> round down 0, then +5 => 5
  TEST_ASSERT_EQUAL_UINT8(10, dt.hour);
  // untouched fields
  TEST_ASSERT_EQUAL_UINT8(1, dt.weekday);
  TEST_ASSERT_EQUAL_UINT8(1, dt.day);
  TEST_ASSERT_EQUAL_UINT8(1, dt.month);
  TEST_ASSERT_EQUAL_UINT16(2025, dt.year);
}

void test_timePlus_wrap_midnight() {
  DateTime dt{ .second = 59, .minute = 58, .hour = 23 };
  timePlus(dt);
  TEST_ASSERT_EQUAL_UINT8(0, dt.second);
  TEST_ASSERT_EQUAL_UINT8(0, dt.minute);  // 58 -> 55 -> +5 => 0
  TEST_ASSERT_EQUAL_UINT8(0, dt.hour);    // 23 -> 0
}

void test_timeMinus_round_down_and_subtract() {
  DateTime dt{ .second = 11, .minute = 3, .hour = 10 };
  timeMinus(dt);
  TEST_ASSERT_EQUAL_UINT8(0, dt.second);
  TEST_ASSERT_EQUAL_UINT8(55, dt.minute);  // 3 -> 0 -> -5 => 55
  TEST_ASSERT_EQUAL_UINT8(9, dt.hour);     // borrow
}

void test_timeMinus_borrow_hour_and_wrap() {
  DateTime dt1{ .second = 0, .minute = 0, .hour = 0 };
  timeMinus(dt1);
  TEST_ASSERT_EQUAL_UINT8(55, dt1.minute);
  TEST_ASSERT_EQUAL_UINT8(23, dt1.hour);

  DateTime dt2{ .second = 7, .minute = 0, .hour = 12 };
  timeMinus(dt2);
  TEST_ASSERT_EQUAL_UINT8(0, dt2.second);
  TEST_ASSERT_EQUAL_UINT8(55, dt2.minute);
  TEST_ASSERT_EQUAL_UINT8(11, dt2.hour);
}

void test_clockutils() {
  RUN_TEST(test_cycle_brightness);
  RUN_TEST(test_timePlus_round_down_and_add);
  RUN_TEST(test_timePlus_wrap_midnight);
  RUN_TEST(test_timeMinus_round_down_and_subtract);
  RUN_TEST(test_timeMinus_borrow_hour_and_wrap);
}
