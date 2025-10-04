#include <KoreanClockFace.h>

#include <unity.h>

// Minimal fakes / stubs for dependencies so we can instantiate KoreanClockFace.

class FakeLedMatrix : public LedMatrix {
public:
  void begin() override {}
  void show() override {}
  void clear() override {}
  void setBrightness(uint8_t) override {}
  void setCell(LedPosition &, LedColor &) override {}
};

class FakeClock : public Clock {
public:
  DateTime dt{ 0, 0, 0, 0, 1, 1, 2000 };
  DateTime readDateTime() override {
    return dt;
  }
  void setDateTime(DateTime &newDt) override {
    dt = newDt;
  }
};

void test_can_instantiate() {
  FakeLedMatrix matrix;
  FakeClock clock;
  KoreanClockFace face(matrix, clock);
  // At this point we only verify that construction does not crash.
  TEST_ASSERT_TRUE(true);
}

void setUp() {}
void tearDown() {}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_can_instantiate);
  return UNITY_END();
}
