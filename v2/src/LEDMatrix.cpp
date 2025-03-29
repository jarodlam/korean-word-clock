#include "LEDMatrix.h"

LEDMatrix::LEDMatrix(uint8_t width, uint8_t height, uint8_t ledsPerCell, int16_t pin) {
  this->width = width;
  this->height = height;
  this->ledsPerCell = ledsPerCell;
  neopixel = new Adafruit_NeoPixel(numLeds(), pin, NEO_GRBW + NEO_KHZ800);
}

void LEDMatrix::begin() {
  neopixel->begin();
  neopixel->clear();
  neopixel->show();
}

void LEDMatrix::show() {
  return neopixel->show();
}

void LEDMatrix::clear() {
  return neopixel->clear();
}

void LEDMatrix::setBrightness(uint8_t v) {
  return neopixel->setBrightness(v);
}

void LEDMatrix::setCell(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  Serial.println("setCell");
  // Determine cell position along LED strip
  uint16_t verticalOffset = y * width;
  uint16_t horizontalOffset;
  if (y % 2 == 0) {
    // Even row, left-to-right
    horizontalOffset = x;
  } else {
    // Odd row, right-to-left
    horizontalOffset = (width - 1) - x;
  }
  uint16_t firstPos = (verticalOffset + horizontalOffset) * ledsPerCell; 

  // Set all leds
  for (uint8_t i = 0; i < ledsPerCell; i++) {
    uint16_t pos = firstPos + i;
    Serial.println(pos);
    neopixel->setPixelColor(pos, neopixel->Color(r, g, b, w));
  }
}

uint16_t LEDMatrix::numLeds() {
  return width * height * ledsPerCell;
}
