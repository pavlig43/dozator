#pragma once

#include "ir.h"
#include "servo_memory.h"
#include "servo_motor.h"

enum class AngleSettingKind : byte {
  OPEN_ANGLE,
  CLOSED_ANGLE,
  SLOW_ANGLE,
  STOP_WEIGHT
};

// Сценарий настройки заслонки.
// При настройке углов сразу отправляет выбранный угол в Servo,
// чтобы пользователь видел движение заслонки во время UP/DOWN.
class AngleSettings {
public:
  AngleSettings(Servo& servo, ServoMemory& memory);
  void init(); // Загружает углы из EEPROM и ставит серво в закрытое положение.
  void begin(); // Начинает настройку с openAngle.
  void end(); // Сохраняет настройки и возвращает серво в закрытое положение.
  void loop(); // Даёт физическому Servo применить targetAngle.
  bool handleButton(Button button); // Возвращает true, если экран нужно обновить.
  bool isActive() const; // false после сохранения slowAngle.
  AngleSettingKind currentKind() const; // Какая настройка сейчас редактируется.
  int currentValue() const; // Текущее значение настройки.

private:
  Servo& servo;
  ServoMemory& memory;
  bool active = false; // true, пока открыт экран настройки углов.
  byte currentIndex = 0; // Индекс текущего угла в списке настройки.

  void moveToIndex(byte nextIndex); // Переключает шаг и сразу показывает его серво.
  void selectByButton(Button button); // Циклически выбирает угол по PREV/NEXT.
  AngleSettingKind currentSettingKind() const; // Тип настройки для текущего индекса.
  bool currentSettingIsAngle() const;
  AngleKind currentAngleKind() const;
};
