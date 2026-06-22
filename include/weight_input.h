#pragma once

#include <Arduino.h>
#include "ir.h"

// WeightInput хранит пять цифр целевого веса и положение курсора.
// Он не знает про LCD, EEPROM и экраны: только редактирует число по кнопкам.
class WeightInput {
public:
  void setTargetWeight(unsigned long weight); // Раскладывает число на 5 цифр экрана.
  void handleButton(Button button); // LEFT/RIGHT/CLEAR/цифры меняют локальное состояние.
  unsigned long targetWeight() const; // Собирает пять цифр обратно в число.
  const byte* getDigits() const; // Доступ для Display/экрана без копирования массива.
  byte getCursor() const; // Текущая позиция редактирования.

private:
  byte digits[5] = {0, 0, 0, 5, 0}; // Значение по умолчанию: 50 грамм.
  byte cursor = 0; // Индекс редактируемой цифры 0..4.
};
