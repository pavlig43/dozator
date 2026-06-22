#pragma once
#include <Arduino.h>
#include "servo_memory.h"

// Display - единственный слой, который напрямую рисует на LCD.
// Экраны говорят ему, что показать, но не работают с LiquidCrystal_I2C напрямую.
class Display {
public:
  void init(); // Инициализирует LCD и показывает стартовую надпись.
  void showLoading(); // Экран загрузки.
  void showTare(); // Короткое сообщение во время тарирования.
  void showWeightInput(const byte digits[5], byte cursor); // Полный экран ввода веса.
  void updateDigits(const byte digits[5]); // Обновляет только строку цифр.
  void updateCursor(byte cursor); // Обновляет только указатель курсора.
  void showWork(long currentWeight, unsigned long targetWeight, bool showChHint); // Полный рабочий экран.
  void updateCurrentWeight(long currentWeight, bool showChHint); // Обновляет только текущий вес.
  void showAngleSettings(AngleKind angleKind, int angle); // Экран текущего угла настройки.
};
