#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lsd.h"
// SCL-> A5
// SDA-> A4

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Инициализирует LCD и включает подсветку.
void Display::init() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  showLoading();
}

// Показывает экран загрузки при старте.
void Display::showLoading() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading...");

}

// Показывает экран тарирования весов.
void Display::showTare() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Tare...");
}

// Показывает экран ввода целевого веса.
void Display::showWeightInput(const byte digits[5], byte cursor) {
  updateDigits(digits);
  updateCursor(cursor);
}

// Обновляет только верхнюю строку с цифрами целевого веса.
void Display::updateDigits(const byte digits[5]) {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 5; i++) {
    lcd.print(digits[i]);
  }
  lcd.print("           ");
}

// Обновляет только нижнюю строку с указателем курсора.
void Display::updateCursor(byte cursor) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(cursor, 1);
  lcd.print("^");
}

// Полностью рисует рабочий экран: текущий вес с подсказкой CH и целевой вес.
void Display::showWork(long currentWeight, unsigned long targetWeight, bool showChHint) {
  lcd.clear();
  updateCurrentWeight(currentWeight, showChHint);
  lcd.setCursor(0, 1);
  lcd.print(targetWeight);
}

// Обновляет только верхнюю строку с текущим весом и подсказкой CH.
void Display::updateCurrentWeight(long currentWeight, bool showChHint) {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  if (showChHint) {
    lcd.print("CH ");
  }
  lcd.print(currentWeight);
}

// Показывает выбранный этап настройки и текущее значение угла.
void Display::showAngleSettings(AngleKind angleKind, int angle) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (angleKind == AngleKind::OPEN) {
    lcd.print("OPEN ANGLE");
  }
  else if (angleKind == AngleKind::CLOSED) {
    lcd.print("CLOSED ANGLE");
  }
  else {
    lcd.print("SLOW ANGLE");
  }
  lcd.setCursor(0, 1);
  lcd.print(angle);
}
