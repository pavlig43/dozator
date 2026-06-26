#include "angle_settings_screen.h"

AngleSettingsScreen::AngleSettingsScreen(Display& display, AngleSettings& angleSettings, Navigation& navigation)
  : display(display),
    angleSettings(angleSettings),
    navigation(navigation) {
}

void AngleSettingsScreen::enter() {
  // Настройка заслонки нужна не всегда, поэтому EEPROM серво и само серво поднимаем лениво.
  if (!initialized) {
    angleSettings.init();
    initialized = true;
  }

  angleSettings.begin();
  display.showAngleSettings(angleSettings.currentKind(), angleSettings.currentValue());
}

void AngleSettingsScreen::exit() {
  angleSettings.end();
  angleSettings.loop();
}

void AngleSettingsScreen::loop() {
  angleSettings.loop();
}

void AngleSettingsScreen::handleButton(Button button) {
  // AngleSettings возвращает true, если после кнопки нужно обновить LCD.
  const bool changed = angleSettings.handleButton(button);

  if (!changed) {
    return;
  }

  display.showAngleSettings(angleSettings.currentKind(), angleSettings.currentValue());
}
