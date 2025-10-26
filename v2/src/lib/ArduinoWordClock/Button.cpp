#include "Button.h"

Button::Button(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

void Button::update() {
  bool newState = digitalRead(this->pin);

  // Only run on rising/falling edge
  if (newState == this->state) {
    return;
  }

  // Debounce
  unsigned long currentMillis = millis();
  if (currentMillis < prevMillis) {  // millis() overflow
    prevMillis = 0;
  }
  if ((currentMillis - prevMillis) < BUTTON_DEBOUNCE_DELAY) {
    return;
  }

  this->state = newState;
  if (newState == true) {
    consumed = false;
  }
}

bool Button::read() {
  // Only return the true value once, so we only run the action once
  if (this->consumed) {
    return false;
  }
  this->consumed = false;
  return state;
}
