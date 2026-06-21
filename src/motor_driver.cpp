#include <Arduino.h>
#include "motor_driver.h"

#define MOTOR_PWM_PIN 11

bool motorFeeding = false;

void disableMotor() {
  digitalWrite(MOTOR_PWM_PIN, LOW);
}

// Настраивает MOSFET-модуль двигателя и оставляет мотор выключенным при запуске.
void motorSetup() {
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  disableMotor();
}

// Мотор управляется как простой ключ: включён или выключен.
void motorLoop() {
}

// Запоминает, должна ли сейчас работать фактическая подача продукта.
void motorSetFeeding(bool feeding) {
  if (motorFeeding == feeding) {
    return;
  }

  motorFeeding = feeding;

  if (feeding) {
    digitalWrite(MOTOR_PWM_PIN, HIGH);
  }
  else {
    disableMotor();
  }
}
