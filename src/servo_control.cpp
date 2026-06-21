#include <Arduino.h>
#include <EEPROM.h>
#include <LightweightServo.hpp>
#include "servo_control.h"
#include "ir.h"
#include "scale_sensor.h"
#include "motor_driver.h"

// LightweightServo на Arduino Uno работает только с D9 или D10.
// Здесь используем D9 через write9(...).
#define SERVO_PIN 9

#define EEPROM_MARKER_ADDRESS 0
#define EEPROM_OPEN_ANGLE_ADDRESS 1
#define EEPROM_CLOSED_ANGLE_ADDRESS 2
#define EEPROM_FINE_ANGLE_ADDRESS 8
#define EEPROM_FINE_ANGLE_MARKER_ADDRESS 9
#define EEPROM_SETTINGS_MARKER 0xA5
#define EEPROM_FINE_ANGLE_MARKER 0xF1
#define DEFAULT_ANGLE 90
#define MIN_ANGLE 10
#define MAX_ANGLE 170
#define SMALL_REMAINING_LIMIT_GRAMS 200
#define MEDIUM_REMAINING_LIMIT_GRAMS 400
#define MAIN_FEED_STOP_MARGIN_GRAMS 50
#define SETTLE_TIME_MS 1000UL
#define FINE_FEED_TIME_MS 1000UL

enum DosingState {
  DOSING_IDLE,
  DOSING_MAIN_FEED,
  DOSING_MAIN_SETTLE,
  DOSING_FINE_FEED,
  DOSING_FINE_SETTLE
};

// Последний угол, который уже был отправлен в серву.
int lastAngle = -1;
DosingState dosingState = DOSING_IDLE;
unsigned long dosingStateStartedAt = 0;
// Сохранённые положения заслонки.
int openAngle = DEFAULT_ANGLE;
int closedAngle = DEFAULT_ANGLE;
int fineAngle = DEFAULT_ANGLE;
// Выбранный угол во время настройки.
ServoAngleType activeAngle = SERVO_OPEN_ANGLE;
bool angleSetupActive = false;

// Переводит заслонку в указанный угол, не повторяя одинаковые команды серве.
void writeServoAngle(int angle) {
  if (angle != lastAngle) {
    write9(angle);
    lastAngle = angle;
  }
}

// Выбирает угол основной подачи по оставшемуся до цели весу:
// треть диапазона, половина диапазона или полное открытие.
int getMainFeedAngle(long remainingWeight) {
  int angleRange = closedAngle - openAngle;

  if (remainingWeight < SMALL_REMAINING_LIMIT_GRAMS) {
    return closedAngle - angleRange / 3;
  }

  if (remainingWeight < MEDIUM_REMAINING_LIMIT_GRAMS) {
    return closedAngle - angleRange / 2;
  }

  return openAngle;
}

// Переводит автомат в новое состояние и запускает отсчёт времени этого этапа.
void setDosingState(DosingState newState) {
  dosingState = newState;
  dosingStateStartedAt = millis();
}

// Завершает дозирование: закрывает заслонку и выключает двигатель.
void finishDosing() {
  setDosingState(DOSING_IDLE);
  writeServoAngle(closedAngle);
  motorSetFeeding(false);
}

// Запускает основную подачу с углом, выбранным по заданному весу.
void servoStartCycle() {
  setDosingState(DOSING_MAIN_FEED);
  writeServoAngle(getMainFeedAngle(irGetTargetNumber() - scaleGetWeightGrams()));
  motorSetFeeding(true);
}

// Немедленно останавливает подачу, закрывает заслонку и выключает двигатель.
void servoStopCycle() {
  setDosingState(DOSING_IDLE);
  writeServoAngle(closedAngle);
  motorSetFeeding(false);
}

// Сообщает экрану, выполняется ли сейчас один из этапов дозирования.
bool servoIsOpening() {
  return dosingState != DOSING_IDLE;
}

// Загружает настроенные углы из EEPROM или оставляет значения по умолчанию.
// Взаимное положение углов здесь намеренно не проверяется.
void servoLoadSettings() {
  if (EEPROM.read(EEPROM_MARKER_ADDRESS) == EEPROM_SETTINGS_MARKER) {
    openAngle = constrain(EEPROM.read(EEPROM_OPEN_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
    closedAngle = constrain(EEPROM.read(EEPROM_CLOSED_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
  }
  else {
    openAngle = DEFAULT_ANGLE;
    closedAngle = DEFAULT_ANGLE;
  }

  if (EEPROM.read(EEPROM_FINE_ANGLE_MARKER_ADDRESS) == EEPROM_FINE_ANGLE_MARKER) {
    fineAngle = constrain(EEPROM.read(EEPROM_FINE_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
  }
  else {
    fineAngle = closedAngle;
  }
}

// Сохраняет настроенные углы в EEPROM только при изменении байтов.
void servoSaveSettings() {
  EEPROM.update(EEPROM_OPEN_ANGLE_ADDRESS, openAngle);
  EEPROM.update(EEPROM_CLOSED_ANGLE_ADDRESS, closedAngle);
  EEPROM.update(EEPROM_FINE_ANGLE_ADDRESS, fineAngle);
  EEPROM.update(EEPROM_FINE_ANGLE_MARKER_ADDRESS, EEPROM_FINE_ANGLE_MARKER);
  EEPROM.update(EEPROM_MARKER_ADDRESS, EEPROM_SETTINGS_MARKER);
}

// Возвращает один из сохранённых углов заслонки.
int servoGetAngle(ServoAngleType angleType) {
  if (angleType == SERVO_OPEN_ANGLE) {
    return openAngle;
  }

  if (angleType == SERVO_CLOSED_ANGLE) {
    return closedAngle;
  }

  return fineAngle;
}

// Начинает настройку выбранного положения и сразу показывает его механически.
void servoSetActiveAngle(ServoAngleType angleType) {
  activeAngle = angleType;
  angleSetupActive = true;
  writeServoAngle(servoGetAngle(activeAngle));
}

// Начинает настройку досыпки с текущего закрытого положения.
void servoResetFineAngleToClosed() {
  fineAngle = closedAngle;
}

// Изменяет настраиваемый угол на указанное число градусов и двигает серву.
void servoChangeActiveAngle(int delta) {
  int newAngle = constrain(servoGetAngle(activeAngle) + delta, MIN_ANGLE, MAX_ANGLE);

  if (activeAngle == SERVO_OPEN_ANGLE) {
    openAngle = newAngle;
  }
  else if (activeAngle == SERVO_CLOSED_ANGLE) {
    closedAngle = newAngle;
  }
  else {
    fineAngle = newAngle;
  }

  writeServoAngle(newAngle);
}

// Завершает настройку, закрывает заслонку и гарантированно отключает подачу.
void servoFinishAngleSetup() {
  angleSetupActive = false;
  dosingState = DOSING_IDLE;
  writeServoAngle(closedAngle);
  motorSetFeeding(false);
}

// Загружает углы при запуске и устанавливает заслонку в закрытое положение.
void servoSetup() {
  servoLoadSettings();
  writeServoAngle(closedAngle);
}

// Выполняет текущий этап дозирования без delay():
// основную подачу, стабилизацию, точную досыпку или завершение.
void servoLoop() {
  if (angleSetupActive) {
    return;
  }

  long targetWeight = irGetTargetNumber();
  long currentWeight = scaleGetWeightGrams();
  unsigned long stateElapsed = millis() - dosingStateStartedAt;

  if (dosingState != DOSING_IDLE && currentWeight >= targetWeight) {
    finishDosing();
    return;
  }

  switch (dosingState) {
  case DOSING_MAIN_FEED:
    if (currentWeight >= targetWeight - MAIN_FEED_STOP_MARGIN_GRAMS) {
      writeServoAngle(closedAngle);
      motorSetFeeding(false);
      setDosingState(DOSING_MAIN_SETTLE);
    }
    else {
      writeServoAngle(getMainFeedAngle(targetWeight - currentWeight));
    }
    break;

  case DOSING_MAIN_SETTLE:
  case DOSING_FINE_SETTLE:
    if (stateElapsed >= SETTLE_TIME_MS) {
      writeServoAngle(fineAngle);
      motorSetFeeding(true);
      setDosingState(DOSING_FINE_FEED);
    }
    break;

  case DOSING_FINE_FEED:
    if (stateElapsed >= FINE_FEED_TIME_MS) {
      writeServoAngle(closedAngle);
      motorSetFeeding(false);
      setDosingState(DOSING_FINE_SETTLE);
    }
    break;

  case DOSING_IDLE:
    writeServoAngle(closedAngle);
    motorSetFeeding(false);
    break;
  }
}
