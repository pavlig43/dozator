#include <Arduino.h>
#include <EEPROM.h>
#include "target_memory.h"

// EEPROM-карта целевого веса:
// 4    - marker, признак корректно сохранённого веса.
// 5..8 - unsigned long с последним подтверждённым целевым весом.
#define EEPROM_TARGET_MARKER_ADDRESS 4
#define EEPROM_TARGET_VALUE_ADDRESS 5
#define EEPROM_TARGET_MARKER 0x5A

void TargetMemory::init() {
  if (EEPROM.read(EEPROM_TARGET_MARKER_ADDRESS) != EEPROM_TARGET_MARKER) {
    targetWeight = 50;
    return;
  }

  EEPROM.get(EEPROM_TARGET_VALUE_ADDRESS, targetWeight);
}

void TargetMemory::save(unsigned long weight) {
  targetWeight = weight;
  EEPROM.put(EEPROM_TARGET_VALUE_ADDRESS, targetWeight);
  EEPROM.update(EEPROM_TARGET_MARKER_ADDRESS, EEPROM_TARGET_MARKER);
}

unsigned long TargetMemory::weight() const {
  return targetWeight;
}
