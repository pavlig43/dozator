#include <Arduino.h>
#include <Wire.h>
#include <ir.h>
#include "lsd.h"
#include "scale_sensor.h"
#include "servo_control.h"
#include "motor_driver.h"
#include "rgb_led.h"

#define WEIGHT_SCREEN_UPDATE_MS 200UL

// Время последнего обновления верхней строки рабочего экрана.
unsigned long lastWeightScreenUpdate = 0;
// Последние цифры, уже показанные на экране ввода.
byte lastShownDigits[5] = {255, 255, 255, 255, 255};
// Последняя показанная позиция курсора на экране ввода.
byte lastShownCursor = 255;

// Проверяет, нужно ли перерисовать введённый целевой вес.
bool digitsChanged() {
  for (byte i = 0; i < 5; i++) {
    if (digits[i] != lastShownDigits[i]) {
      return true;
    }
  }

  return false;
}

// Запоминает уже показанные цифры для последующего сравнения.
void rememberDigits() {
  for (byte i = 0; i < 5; i++) {
    lastShownDigits[i] = digits[i];
  }
}

// Показывает экран ввода цели и сохраняет его текущее состояние.
void showEditScreen() {
  lsdShowDigits(digits, cursor);
  rememberDigits();
  lastShownCursor = cursor;
}

// Перерисовывает только изменившиеся цифры или положение курсора.
void updateEditScreen() {
  if (digitsChanged()) {
    lsdUpdateDigits(digits);
    rememberDigits();
  }

  if (cursor != lastShownCursor) {
    lsdUpdateCursor(cursor);
    lastShownCursor = cursor;
  }
}

// Показывает RGB-индикацию только для рабочего режима.
void updateRgbStatus() {
  if (irIsAngleSetupActive() || !workScreen) {
    rgbOff();
  }
  else if (servo().isOpening()) {
    rgbBlue();
  }
  else {
    rgbGreen();
  }
}

// Инициализирует дисплей, весы, пульт, сервопривод, двигатель и RGB.
void setup()
{


  lsdSetup();
  lsdShowLoading();
  scaleSetup();
  irSetup();
  servo().setup();
  motorSetup();
  rgbSetup();
  showEditScreen();
}

// Обслуживает устройства и обновляет экран без блокирующих задержек.
void loop() {
  irLoop();
  scaleLoop();
  servo().loop();
  motorLoop();
  updateRgbStatus();

  if (irIsAngleSetupActive()) {
    return;
  }

  if (!workScreen) {
    updateEditScreen();
    return;
  }

  // При входе в рабочий экран рисуем его целиком.
  if (lastShownCursor != 255) {
    lsdShowWorkScreen(scaleGetWeightGrams(), irGetTargetNumber(), !servo().isOpening());
    lastWeightScreenUpdate = millis();
    lastShownCursor = 255;
  }

  // В рабочем экране периодически обновляем только текущий вес.
  if (millis() - lastWeightScreenUpdate >= WEIGHT_SCREEN_UPDATE_MS) {
    lastWeightScreenUpdate = millis();
    lsdUpdateCurrentWeight(scaleGetWeightGrams(), !servo().isOpening());
  }
}
