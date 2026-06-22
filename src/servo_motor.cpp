#include <Arduino.h>
#include <LightweightServo.hpp>
#include "servo_motor.h"

// LightweightServo на Arduino Uno работает только с D9 или D10.
// Здесь используется D9 через write9(...).
#define SERVO_PIN 9
#define MIN_ANGLE 10
#define MAX_ANGLE 170

void Servo::init(int initialAngle) {
  setAngle(initialAngle);
  loop();
}

// Физическая запись в серво выполняется только когда целевой угол изменился.
void Servo::loop() {
  if (currentAngle == targetAngle) {
    return;
  }

  write9(targetAngle);
  currentAngle = targetAngle;
}

void Servo::setAngle(int angle) {
  // Ограничение защищает механику от упора в крайние положения.
  targetAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);
}

int Servo::getTargetAngle() const {
  return targetAngle;
}
