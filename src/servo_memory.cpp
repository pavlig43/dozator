#include <Arduino.h>
#include <EEPROM.h>
#include "servo_memory.h"

// EEPROM-карта углов серво:
// 0    - marker, признак корректно сохранённых углов.
// 1    - openAngle, угол полного открытия заслонки.
// 2    - closedAngle, угол полностью закрытой заслонки.
// 3    - slowAngle, угол медленной досыпки.
// Углы занимают по одному байту, поэтому рабочий диапазон ограничен 10..170.
#define EEPROM_SERVO_MARKER_ADDRESS 0
#define EEPROM_SERVO_OPEN_ADDRESS 1
#define EEPROM_SERVO_CLOSED_ADDRESS 2
#define EEPROM_SERVO_SLOW_ADDRESS 3
#define EEPROM_SERVO_MARKER 0xA5

#define DEFAULT_ANGLE 90
#define MIN_ANGLE 10
#define MAX_ANGLE 170

void ServoMemory::init() {
  if (EEPROM.read(EEPROM_SERVO_MARKER_ADDRESS) != EEPROM_SERVO_MARKER) {
    openValue = DEFAULT_ANGLE;
    closedValue = DEFAULT_ANGLE;
    slowValue = closedValue;
    return;
  }

  openValue = constrain(EEPROM.read(EEPROM_SERVO_OPEN_ADDRESS), MIN_ANGLE, MAX_ANGLE);
  closedValue = constrain(EEPROM.read(EEPROM_SERVO_CLOSED_ADDRESS), MIN_ANGLE, MAX_ANGLE);
  slowValue = constrain(EEPROM.read(EEPROM_SERVO_SLOW_ADDRESS), MIN_ANGLE, MAX_ANGLE);
}

void ServoMemory::save() const {
  EEPROM.update(EEPROM_SERVO_OPEN_ADDRESS, openValue);
  EEPROM.update(EEPROM_SERVO_CLOSED_ADDRESS, closedValue);
  EEPROM.update(EEPROM_SERVO_SLOW_ADDRESS, slowValue);
  EEPROM.update(EEPROM_SERVO_MARKER_ADDRESS, EEPROM_SERVO_MARKER);
}

int ServoMemory::get(AngleKind kind) const {
  if (kind == AngleKind::OPEN) {
    return openValue;
  }

  if (kind == AngleKind::CLOSED) {
    return closedValue;
  }

  return slowValue;
}

void ServoMemory::set(AngleKind kind, int angle) {
  const int safeAngle = constrain(angle, MIN_ANGLE, MAX_ANGLE);

  if (kind == AngleKind::OPEN) {
    openValue = safeAngle;
  }
  else if (kind == AngleKind::CLOSED) {
    closedValue = safeAngle;
  }
  else {
    slowValue = safeAngle;
  }
}

int ServoMemory::openAngle() const {
  return openValue;
}

int ServoMemory::closedAngle() const {
  return closedValue;
}

int ServoMemory::slowAngle() const {
  return slowValue;
}
