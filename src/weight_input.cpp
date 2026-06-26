#include <Arduino.h>
#include <string.h>
#include "weight_input.h"

void WeightInput::setTargetWeight(unsigned long weight) {
  for (int i = 4; i >= 0; i--) {
    digits[i] = weight % 10;
    weight /= 10;
  }
}

void WeightInput::handleButton(Button button) {
  if (button == Button::PREV) {
    cursor = (cursor + 4) % 5;
    return;
  }

  if (button == Button::NEXT) {
    cursor = (cursor + 1) % 5;
    return;
  }

  if (button == Button::CLEAR) {
    memset(digits, 0, sizeof(digits));
    cursor = 0;
    return;
  }

  const int digit = IrRemote::digit(button);
  if (digit >= 0) {
    digits[cursor] = digit;
  }
}

unsigned long WeightInput::targetWeight() const {
  unsigned long value = 0;

  for (byte i = 0; i < 5; i++) {
    value = value * 10 + digits[i];
  }

  return value;
}

const byte* WeightInput::getDigits() const {
  return digits;
}

byte WeightInput::getCursor() const {
  return cursor;
}
