#pragma once

#include "ir.h"
#include "servo_memory.h"
#include "servo_motor.h"

// Сценарий настройки углов заслонки.
// Хранит только шаг сценария и сразу отправляет выбранный угол в Servo,
// чтобы пользователь видел движение заслонки во время UP/DOWN.
class AngleSettings {
public:
  AngleSettings(Servo& servo, ServoMemory& memory);
  void init(); // Загружает углы из EEPROM и ставит серво в закрытое положение.
  void begin(); // Начинает настройку с openAngle.
  void loop(); // Даёт физическому Servo применить targetAngle.
  bool handleButton(Button button); // Возвращает true, если экран нужно обновить.
  bool isActive() const; // false после сохранения slowAngle.
  AngleKind currentKind() const; // Какой угол сейчас редактируется.
  int currentAngle() const; // Текущее значение редактируемого угла.

private:
  enum class Step : byte {
    NONE,   // Настройка не запущена.
    OPEN,   // Редактируется угол полного открытия.
    CLOSED, // Редактируется угол закрытия.
    SLOW    // Редактируется угол медленной досыпки.
  };

  Servo& servo;
  ServoMemory& memory;
  Step step = Step::NONE;

  void moveTo(Step nextStep); // Переключает шаг и сразу показывает его серво.
  AngleKind kindForStep() const; // Переводит шаг сценария в тип угла для памяти/LCD.
};
