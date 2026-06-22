#include <Arduino.h>
#include "app.h"

App::App()
  // Здесь только связываем объекты между собой.
  // Реальная инициализация железа находится в init()/enter() нужного экрана.
  : angleSettings(servo, servoMemory),
    dosing(servo, servoMemory, scale, motor, targetMemory),
    weightInputScreen(display, targetMemory, navigation),
    angleSettingsScreen(display, angleSettings, navigation),
    workScreen(display, scale, rgb, dosing, targetMemory, navigation, servo) {
}

App& app() {
  // Локальный static создаётся при первом обращении из Arduino setup().
  // Так в заголовке нет глобальной переменной App, и IDE не ругается на неё.
  static App instance;
  return instance;
}

void App::init() {
  // Host поднимает только то, без чего нельзя показать первый экран и читать кнопки.
  // Остальное лениво инициализируют экраны при первом входе.
  display.init();
  remote.init();
  enterCurrentScreen();
}

void App::loop() {
  const Button button = remote.read();

  // Порядок важен: экран сначала получает кнопку и может запросить переход.
  // Потом App применяет переход, и loop() вызывается уже у актуального экрана.
  handleButton(button);
  applyNavigation();
  loopCurrentScreen();
}

void App::handleButton(Button button) {
  if (button == Button::NONE) {
    return;
  }

  switch (navigation.current()) {
  case ScreenId::WEIGHT_INPUT:
    weightInputScreen.handleButton(button);
    break;

  case ScreenId::ANGLE_SETTINGS:
    angleSettingsScreen.handleButton(button);
    break;

  case ScreenId::WORK:
    workScreen.handleButton(button);
    break;
  }
}

void App::loopCurrentScreen() {
  switch (navigation.current()) {
  case ScreenId::WEIGHT_INPUT:
    weightInputScreen.loop();
    break;

  case ScreenId::ANGLE_SETTINGS:
    angleSettingsScreen.loop();
    break;

  case ScreenId::WORK:
    workScreen.loop();
    break;
  }
}

void App::enterCurrentScreen() {
  switch (navigation.current()) {
  case ScreenId::WEIGHT_INPUT:
    weightInputScreen.enter();
    break;

  case ScreenId::ANGLE_SETTINGS:
    angleSettingsScreen.enter();
    break;

  case ScreenId::WORK:
    workScreen.enter();
    break;
  }
}

void App::exitCurrentScreen() {
  // Пока только рабочий экран имеет активные процессы, которые надо остановить:
  // дозирование, мотор, RGB и целевой угол серво.
  if (navigation.current() == ScreenId::WORK) {
    workScreen.exit();
  }
}

void App::applyNavigation() {
  // Экраны не переключаются сами напрямую: они только просят Navigation открыть экран.
  // Commit происходит здесь, чтобы enter/exit всегда выполнялись в одном месте.
  if (!navigation.hasPendingChange()) {
    return;
  }

  exitCurrentScreen();
  navigation.commit();
  enterCurrentScreen();
}
