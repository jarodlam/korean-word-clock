#include "Sk6812LedMatrix.h"

Sk6812LedMatrix::Sk6812LedMatrix(uint8_t width, uint8_t height, uint8_t ledsPerCell, int16_t pin) {
  this->width = width;
  this->height = height;
  this->ledsPerCell = ledsPerCell;
  neopixel = new Adafruit_NeoPixel(numLeds(), pin, NEO_GRBW + NEO_KHZ800);
}

void Sk6812LedMatrix::begin() {
  neopixel->begin();
  neopixel->clear();
  neopixel->show();
}

void Sk6812LedMatrix::show() { return neopixel->show(); }

void Sk6812LedMatrix::clear() { return neopixel->clear(); }

void Sk6812LedMatrix::setBrightness(uint8_t v) { return neopixel->setBrightness(v); }

void Sk6812LedMatrix::setCell(LedPosition &pos, LedColor &color) {
  // Determine cell position along LED strip
  uint16_t verticalOffset = pos.y * width;
  uint16_t horizontalOffset;
  if (pos.y % 2 == 0) {
    // Even row, left-to-right
    horizontalOffset = pos.x;
  } else {
    // Odd row, right-to-left
    horizontalOffset = (width - 1) - pos.x;
  }
  uint16_t firstPos = (verticalOffset + horizontalOffset) * ledsPerCell;

  // Set all leds
  for (uint8_t i = 0; i < ledsPerCell; i++) {
    uint16_t pos = firstPos + i;
    neopixel->setPixelColor(pos, neopixel->Color(color.r, color.g, color.b, color.w));
  }
}

uint16_t Sk6812LedMatrix::numLeds() { return width * height * ledsPerCell; }
