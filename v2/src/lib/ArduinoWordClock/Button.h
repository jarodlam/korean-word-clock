#pragma once

#include <Arduino.h>

#define BUTTON_DEBOUNCE_DELAY 50  // milliseconds

class Button {
public:
  Button(int pin);
  void update();
  bool read();

private:
  int pin;
  bool state{ false };
  bool prevState{ false };
  unsigned long prevMillis{ 0 };
  bool consumed{ true };
};
