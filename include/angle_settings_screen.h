#pragma once

#include "angle_settings.h"
#include "ir.h"
#include "lsd.h"
#include "navigation.h"

// Экран настройки трёх углов заслонки: open, closed и slow.
// Он не знает EEPROM-деталей: вся работа с углами спрятана в AngleSettings.
class AngleSettingsScreen {
public:
  AngleSettingsScreen(Display& display, AngleSettings& angleSettings, Navigation& navigation);
  void enter(); // Запускает сценарий настройки с openAngle.
  void loop(); // Даёт серво физически применить целевой угол.
  void handleButton(Button button); // UP/DOWN меняют угол, NEXT идёт дальше.

private:
  Display& display;
  AngleSettings& angleSettings;
  Navigation& navigation;
  bool initialized = false; // ServoMemory и Servo поднимаются только при первом входе.
};
