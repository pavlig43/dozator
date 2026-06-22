#pragma once

#include <Arduino.h>

enum class ScreenId : byte {
  WEIGHT_INPUT,    // Экран ввода целевого веса.
  ANGLE_SETTINGS, // Экран настройки углов заслонки.
  WORK            // Рабочий экран дозирования.
};

// Navigation хранит текущий экран и экран, который запросили открыть.
// Это похоже на маленький NavController: экран вызывает openX(),
// а App позже применяет переход через commit().
class Navigation {
public:
  void openWeightInput(); // Запросить переход на ввод веса.
  void openAngleSettings(); // Запросить переход на настройку углов.
  void openWork(); // Запросить переход на рабочий экран.
  ScreenId current() const; // Экран, который сейчас реально активен.
  bool hasPendingChange() const; // Есть ли запрошенный, но ещё не применённый переход.
  void commit(); // Сделать запрошенный экран текущим.

private:
  // currentScreen нужен для loop/handle текущего экрана.
  // requestedScreen нужен, чтобы экран мог безопасно запросить переход во время handleButton().
  ScreenId currentScreen = ScreenId::WEIGHT_INPUT;
  ScreenId requestedScreen = ScreenId::WEIGHT_INPUT;
};
