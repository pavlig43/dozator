#include <Arduino.h>
#include "motor_driver.h"

#define MOTOR_PWM_PIN 11

bool motorFeeding = false;

void disableMotor() {
  digitalWrite(MOTOR_PWM_PIN, LOW);
}

// Настраивает MOSFET-модуль двигателя и оставляет мотор выключенным при запуске.
void Motor::init() {
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  disableMotor();
}

// Мотор управляется как простой ключ: включён или выключен.
void Motor::loop() {
}

// Запоминает, должна ли сейчас работать фактическая подача продукта.
void Motor::setFilling(bool filling) {
  if (motorFeeding == filling) {
    return;
  }

  motorFeeding = filling;

  if (filling) {
    digitalWrite(MOTOR_PWM_PIN, HIGH);
  }
  else {
    disableMotor();
  }
}
