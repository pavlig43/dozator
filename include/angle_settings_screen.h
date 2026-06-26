#pragma once

#include "angle_settings.h"
#include "ir.h"
#include "lsd.h"
#include "navigation.h"

// Экран настройки заслонки: open, closed, slow и вес перехода к досыпке.
// Он не знает EEPROM-деталей: вся работа с настройками спрятана в AngleSettings.
class AngleSettingsScreen {
public:
  AngleSettingsScreen(Display& display, AngleSettings& angleSettings, Navigation& navigation);
  void enter(); // Запускает сценарий настройки с openAngle.
  void exit(); // Сохраняет настройки и закрывает заслонку при уходе с экрана.
  void loop(); // Даёт серво физически применить целевой угол.
  void handleButton(Button button); // UP/DOWN меняют значение, NEXT идёт дальше.

private:
  Display& display;
  AngleSettings& angleSettings;
  Navigation& navigation;
  bool initialized = false; // ServoMemory и Servo поднимаются только при первом входе.
};
