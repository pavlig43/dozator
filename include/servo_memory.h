#pragma once

#include <Arduino.h>

enum class AngleKind : byte {
  OPEN,   // Полное открытие заслонки.
  CLOSED, // Полное закрытие заслонки.
  SLOW    // Малое открытие для медленной досыпки.
};

// ServoMemory хранит три угла заслонки в EEPROM.
// Совместимость со старой картой EEPROM не сохраняется: актуальная карта описана в .cpp.
class ServoMemory {
public:
  void init(); // Загружает углы или выставляет значения по умолчанию.
  void save() const; // Сохраняет все три угла и marker.
  int get(AngleKind kind) const; // Универсальное чтение угла по типу.
  void set(AngleKind kind, int angle); // Записывает угол с ограничением безопасного диапазона.
  int openAngle() const; // Удобный getter для автомата дозирования.
  int closedAngle() const; // Удобный getter для закрытого положения.
  int slowAngle() const; // Удобный getter для режима досыпки.

private:
  int openValue = 90;
  int closedValue = 90;
  int slowValue = 90;
};
