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

#define EEPROM_MARKER_ADDRESS 0 // Адрес признака сохранённых настроек серво.
#define EEPROM_SETTINGS_MARKER_VALUE 0xA5 // Значение признака сохранённых настроек серво.
#define EEPROM_OPEN_ANGLE_ADDRESS 1 // Адрес угла полного открытия заслонки.
#define EEPROM_CLOSED_ANGLE_ADDRESS 2 // Адрес угла закрытой заслонки.
#define EEPROM_SLOW_ANGLE_ADDRESS 3 // Адрес угла медленной досыпки.

#define DEFAULT_ANGLE 90 // Угол по умолчанию, если настройки ещё не сохранены.
#define MIN_ANGLE 10 // Минимальный разрешённый угол, чтобы не упирать механику.
#define MAX_ANGLE 170 // Максимальный разрешённый угол, чтобы не упирать механику.

#define SMALL_REMAINING_LIMIT_GRAMS 200 // Остаток до цели для открытия примерно на треть диапазона.
#define MEDIUM_REMAINING_LIMIT_GRAMS 400 // Остаток до цели для открытия примерно на половину диапазона.
#define MAIN_FILL_STOP_MARGIN_GRAMS 50 // Запас до цели, где основная подача останавливается.

#define WAIT_TIME_MS 1000UL // Пауза после остановки подачи для стабилизации веса.
#define SLOW_FILL_TIME_MS 1000UL // Длительность одного этапа медленной досыпки.

static ServoControl servoInstance;

ServoControl& servo() {
  return servoInstance;
}

// Общие операции серво.

// Отправляет угол в серву только при реальном изменении команды.
void ServoControl::writeAngle(const int angle) {
  if (angle != lastAngle) {
    write9(angle);
    lastAngle = angle;
  }
}

void ServoControl::setup() {
  loadSettings();
  writeAngle(closedAngle);
}

bool ServoControl::isOpening() const {
  return dosingState != DOSING_IDLE;
}

// Настройки углов в EEPROM.

// Углы хранятся в EEPROM по одному байту, поэтому диапазон ограничен 10..170.
void ServoControl::loadSettings() {
  if (EEPROM.read(EEPROM_MARKER_ADDRESS) == EEPROM_SETTINGS_MARKER_VALUE) {
    openAngle = constrain(EEPROM.read(EEPROM_OPEN_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
    closedAngle = constrain(EEPROM.read(EEPROM_CLOSED_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
    slowAngle = constrain(EEPROM.read(EEPROM_SLOW_ANGLE_ADDRESS), MIN_ANGLE, MAX_ANGLE);
  }
  else {
    openAngle = DEFAULT_ANGLE;
    closedAngle = DEFAULT_ANGLE;
    slowAngle = closedAngle;
  }
}

void ServoControl::saveSettings() const {
  EEPROM.update(EEPROM_OPEN_ANGLE_ADDRESS, openAngle);
  EEPROM.update(EEPROM_CLOSED_ANGLE_ADDRESS, closedAngle);
  EEPROM.update(EEPROM_SLOW_ANGLE_ADDRESS, slowAngle);
  EEPROM.update(EEPROM_MARKER_ADDRESS, EEPROM_SETTINGS_MARKER_VALUE);
}

// Ручная настройка углов заслонки.

int ServoControl::getAngle(const ServoControl::AngleType angleType) const {
  if (angleType == OPEN_ANGLE) {
    return openAngle;
  }

  if (angleType == CLOSED_ANGLE) {
    return closedAngle;
  }

  return slowAngle;
}

void ServoControl::setActiveAngle(const ServoControl::AngleType angleType) {
  activeAngle = angleType;
  angleSetupActive = true;
  writeAngle(getAngle(activeAngle));
}

void ServoControl::resetSlowAngleToClosed() {
  slowAngle = closedAngle;
}

void ServoControl::changeActiveAngle(int delta) {
  int newAngle = constrain(getAngle(activeAngle) + delta, MIN_ANGLE, MAX_ANGLE);

  if (activeAngle == OPEN_ANGLE) {
    openAngle = newAngle;
  }
  else if (activeAngle == CLOSED_ANGLE) {
    closedAngle = newAngle;
  }
  else {
    slowAngle = newAngle;
  }

  writeAngle(newAngle);
}

void ServoControl::finishAngleSetup() {
  angleSetupActive = false;
  setDosingState(DOSING_IDLE);
  writeAngle(closedAngle);
  motorSetFeeding(false);
}

// Рабочий автомат дозирования.

// Выбирает угол основной подачи: чем меньше осталось до цели, тем сильнее прикрыта заслонка.
int ServoControl::getMainFillAngle(const long remainingWeight) const {
  const int angleRange = closedAngle - openAngle;

  if (remainingWeight < SMALL_REMAINING_LIMIT_GRAMS) {
    return closedAngle - angleRange / 3;
  }

  if (remainingWeight < MEDIUM_REMAINING_LIMIT_GRAMS) {
    return closedAngle - angleRange / 2;
  }

  return openAngle;
}

// При входе во временное состояние таймер сбрасывается от момента перехода.
void ServoControl::setDosingState(const DosingState newState) {
  dosingState = newState;

  if (newState == DOSING_MAIN_WAIT || newState == DOSING_SLOW_WAIT) {
    waitTimer.reset();
  }
  else if (newState == DOSING_SLOW_FILL) {
    slowFillTimer.reset();
  }
}

void ServoControl::startCycle() {
  setDosingState(DOSING_MAIN_FILL);
  motorSetFeeding(true);
}

void ServoControl::stopCycle() {
  setDosingState(DOSING_IDLE);
  writeAngle(closedAngle);
  motorSetFeeding(false);
}

// Автомат дозирования: основная подача, осадка веса, досыпка и повторная осадка до цели.
void ServoControl::loop() {
  // Во время настройки углов loop() не должен возвращать заслонку в рабочее состояние.
  if (angleSetupActive) {
    return;
  }

  const long targetWeight = irGetTargetNumber();
  const long currentWeight = scaleGetWeightGrams();

  // Цель проверяем только во время запущенного дозирования, чтобы idle не вызывал stopCycle() каждый loop().
  if (dosingState != DOSING_IDLE && currentWeight >= targetWeight) {
    stopCycle();
    return;
  }

  switch (dosingState) {
  case DOSING_MAIN_FILL:
    if (currentWeight >= targetWeight - MAIN_FILL_STOP_MARGIN_GRAMS) {
      writeAngle(closedAngle);
      motorSetFeeding(false);
      setDosingState(DOSING_MAIN_WAIT);
    }
    else {
      writeAngle(getMainFillAngle(targetWeight - currentWeight));
    }
    break;

  case DOSING_MAIN_WAIT:
  case DOSING_SLOW_WAIT:
    if (waitTimer.ready()) {
      writeAngle(slowAngle);
      motorSetFeeding(true);
      setDosingState(DOSING_SLOW_FILL);
    }
    break;

  case DOSING_SLOW_FILL:
    if (slowFillTimer.ready()) {
      writeAngle(closedAngle);
      motorSetFeeding(false);
      setDosingState(DOSING_SLOW_WAIT);
    }
    break;

  case DOSING_IDLE:
    writeAngle(closedAngle);
    motorSetFeeding(false);
    break;
  }
}
