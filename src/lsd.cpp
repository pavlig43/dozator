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

// Показывает меню выбора экрана. PREV/NEXT листают, PLAY открывает выбранный экран.
void Display::showMenu(ScreenId selectedScreen) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select screen");
  lcd.setCursor(0, 1);

  if (selectedScreen == ScreenId::WEIGHT_INPUT) {
    lcd.print("> Weight input");
  }
  else if (selectedScreen == ScreenId::ANGLE_SETTINGS) {
    lcd.print("> Angle setup");
  }
  else if (selectedScreen == ScreenId::SCALE_CALIBRATION) {
    lcd.print("> Calibration");
  }
  else {
    lcd.print("> Work");
  }
}

// Показывает экран тарирования весов.
void Display::showTare() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("Tare...");
}

void Display::showScaleCalibrationEq() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press EQ");
  lcd.setCursor(0, 1);
  lcd.print("to zero");
}

void Display::showScaleCalibrationWeight() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Put 1kg");
  lcd.setCursor(0, 1);
  lcd.print("wait...");
}

void Display::showScaleCalibrationDone() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OK saved");
  lcd.setCursor(0, 1);
  lcd.print("PLAY to menu");
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

// Показывает выбранный этап настройки и текущее значение.
void Display::showAngleSettings(AngleSettingKind kind, int value) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (kind == AngleSettingKind::OPEN_ANGLE) {
    lcd.print("OPEN ANGLE");
  }
  else if (kind == AngleSettingKind::CLOSED_ANGLE) {
    lcd.print("CLOSED ANGLE");
  }
  else if (kind == AngleSettingKind::SLOW_ANGLE) {
    lcd.print("SLOW ANGLE");
  }
  else {
    lcd.print("STOP WEIGHT");
  }
  lcd.setCursor(0, 1);
  lcd.print(value);
}
