#include <unity.h>

void test_KoreanClockFace();
void test_clockutils();

int main(int argc, char** argv) {
  UNITY_BEGIN();
  test_KoreanClockFace();
  test_clockutils();
  return UNITY_END();
}
