#pragma once

#include "ir.h"
#include "motor_driver.h"
#include "scale_sensor.h"
#include "servo_memory.h"
#include "servo_motor.h"
#include "target_memory.h"
#include "tmr.h"

// Рабочий автомат дозирования.
// Он не знает про экран и кнопки: только управляет серво и мотором по текущему весу.
class DosingController {
public:
  DosingController(Servo& servo, ServoMemory& servoMemory, Scale& scale, Motor& motor, TargetMemory& targetMemory);
  void init(); // Готовит серво-память, серво и мотор для рабочего режима.
  void loop(); // Один шаг автомата дозирования.
  void start(); // Запуск основной подачи.
  void stop(); // Безопасная остановка: закрыть заслонку и выключить мотор.
  bool isDosing() const; // true, пока автомат не в IDLE.

private:
  enum class State : byte {
    IDLE,      // Дозирование остановлено: заслонка закрыта, мотор выключен.
    MAIN_FILL, // Быстрая основная подача до подхода к цели.
    WAIT,      // Единая пауза после любой подачи, пока вес стабилизируется.
    SLOW_FILL  // Короткая медленная досыпка через slowAngle.
  };

  Servo& servo;
  ServoMemory& servoMemory;
  Scale& scale;
  Motor& motor;
  TargetMemory& targetMemory;
  State state = State::IDLE; // Текущее состояние автомата.
  Tmr waitTimer; // Пауза стабилизации веса после остановки подачи.
  Tmr slowFillTimer; // Длительность одного короткого импульса досыпки.

  void setState(State nextState); // Меняет состояние и сбрасывает нужный таймер.
  int mainFillAngle(long remainingWeight) const; // Подбирает угол основной подачи по остатку.
};
