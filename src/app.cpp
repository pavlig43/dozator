#include <Arduino.h>
#include "app.h"

namespace {
const ScreenId MENU_ORDER[] = {
  ScreenId::WEIGHT_INPUT,
  ScreenId::ANGLE_SETTINGS,
  ScreenId::SCALE_CALIBRATION,
  ScreenId::WORK
};

const byte MENU_SCREEN_COUNT = sizeof(MENU_ORDER) / sizeof(MENU_ORDER[0]);
}

App::App()
  // Здесь только связываем объекты между собой.
  // Реальная инициализация железа находится в init()/enter() нужного экрана.
  : angleSettings(servo, servoMemory),
    dosing(servo, servoMemory, scale, motor, targetMemory),
    weightInputScreen(display, targetMemory, navigation),
    angleSettingsScreen(display, angleSettings, navigation),
    scaleCalibrationScreen(display, scale),
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
  showMenu();
}

void App::loop() {
  const Button button = remote.read();

  if (menuActive) {
    handleMenuButton(button);
    return;
  }

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

  if (button == Button::CONFIRM) {
    openMenu(navigation.current());
    return;
  }

  switch (navigation.current()) {
  case ScreenId::WEIGHT_INPUT:
    weightInputScreen.handleButton(button);
    break;

  case ScreenId::ANGLE_SETTINGS:
    angleSettingsScreen.handleButton(button);
    break;

  case ScreenId::SCALE_CALIBRATION:
    scaleCalibrationScreen.handleButton(button);
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

  case ScreenId::SCALE_CALIBRATION:
    scaleCalibrationScreen.loop();
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

  case ScreenId::SCALE_CALIBRATION:
    scaleCalibrationScreen.enter();
    break;

  case ScreenId::WORK:
    workScreen.enter();
    break;
  }
}

void App::exitCurrentScreen() {
  if (navigation.current() == ScreenId::WEIGHT_INPUT) {
    weightInputScreen.exit();
  }
  else if (navigation.current() == ScreenId::WORK) {
    workScreen.exit();
  }
  else if (navigation.current() == ScreenId::ANGLE_SETTINGS) {
    angleSettingsScreen.exit();
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
  selectedScreen = navigation.current();
  enterCurrentScreen();
}

void App::showMenu() {
  display.showMenu(selectedScreen);
}

void App::handleMenuButton(Button button) {
  if (button == Button::NONE) {
    return;
  }

  if (button == Button::PREV || button == Button::NEXT) {
    selectMenuByButton(button);
    showMenu();
    return;
  }

  if (button == Button::CONFIRM) {
    openSelectedScreen();
  }
}

void App::openSelectedScreen() {
  menuActive = false;
  openScreen(selectedScreen);
}

void App::openScreen(ScreenId screen) {
  if (screen == navigation.current()) {
    enterCurrentScreen();
    return;
  }

  navigation.open(screen);
  applyNavigation();
}

void App::openMenu(ScreenId screen) {
  exitCurrentScreen();
  selectedScreen = screen;
  menuActive = true;
  showMenu();
}

void App::selectMenuByButton(Button button) {
  selectedScreen = menuScreenByButton(selectedScreen, button);
}

ScreenId App::menuScreenByButton(ScreenId screen, Button button) {
  for (byte i = 0; i < MENU_SCREEN_COUNT; i++) {
    if (MENU_ORDER[i] == screen) {
      if (button == Button::PREV) {
        return MENU_ORDER[(i + MENU_SCREEN_COUNT - 1) % MENU_SCREEN_COUNT];
      }

      return MENU_ORDER[(i + 1) % MENU_SCREEN_COUNT];
    }
  }

  return ScreenId::WEIGHT_INPUT;
}
