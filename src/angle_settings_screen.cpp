#include "angle_settings_screen.h"

AngleSettingsScreen::AngleSettingsScreen(Display& display, AngleSettings& angleSettings, Navigation& navigation)
  : display(display),
    angleSettings(angleSettings),
    navigation(navigation) {
}

void AngleSettingsScreen::enter() {
  // Настройка углов нужна не всегда, поэтому EEPROM серво и само серво поднимаем лениво.
  if (!initialized) {
    angleSettings.init();
    initialized = true;
  }

  angleSettings.begin();
  display.showAngleSettings(angleSettings.currentKind(), angleSettings.currentAngle());
}

void AngleSettingsScreen::loop() {
  angleSettings.loop();
}

void AngleSettingsScreen::handleButton(Button button) {
  // AngleSettings возвращает true, если после кнопки нужно обновить LCD
  // или выйти обратно на экран ввода веса.
  const bool changed = angleSettings.handleButton(button);

  if (!changed) {
    return;
  }

  if (angleSettings.isActive()) {
    display.showAngleSettings(angleSettings.currentKind(), angleSettings.currentAngle());
  }
  else {
    // Сценарий завершён: угол закрытия уже выставлен, возвращаемся к вводу веса.
    angleSettings.loop();
    navigation.openWeightInput();
  }
}
