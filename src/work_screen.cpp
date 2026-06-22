#include "work_screen.h"

#define WEIGHT_SCREEN_UPDATE_MS 200UL

WorkScreen::WorkScreen(Display& display, Scale& scale, Rgb& rgb, DosingController& dosing, TargetMemory& targetMemory, Navigation& navigation, Servo& servo)
  : display(display),
    scale(scale),
    rgb(rgb),
    dosing(dosing),
    targetMemory(targetMemory),
    navigation(navigation),
    servo(servo),
    weightScreenTimer(WEIGHT_SCREEN_UPDATE_MS) {
}

void WorkScreen::init() {
  // Рабочее железо не трогаем при старте приложения, пока пользователь не вошёл в WORK.
  if (initialized) {
    return;
  }

  scale.init();
  rgb.init();
  dosing.init();
  initialized = true;
}

void WorkScreen::enter() {
  init();
  // При входе рисуем весь экран, затем обновляем только текущий вес по таймеру.
  display.showWork(scale.weightGrams(), targetMemory.weight(), !dosing.isDosing());
  weightScreenTimer.reset();
  updateRgb();
  servo.loop();
}

void WorkScreen::exit() {
  // Уход с рабочего экрана всегда должен привести механику в безопасное состояние.
  dosing.stop();
  servo.loop();
  rgb.off();
}

void WorkScreen::loop() {
  // Порядок важен: сначала получить свежий вес, потом принять решение автомата.
  scale.loop();
  dosing.loop();
  servo.loop();
  updateRgb();

  // LCD обновляем реже, чем читаем весы и крутим автомат, потому что запись в LCD медленная.
  if (weightScreenTimer.ready()) {
    display.updateCurrentWeight(scale.weightGrams(), !dosing.isDosing());
  }
}

void WorkScreen::handleButton(Button button) {
  if (button == Button::CONFIRM) {
    // CONFIRM в рабочем режиме означает выход обратно к редактированию цели.
    navigation.openWeightInput();
  }
  else if (button == Button::START) {
    // Перед запуском дозирования тарируем весы, чтобы текущая тара стала нулём.
    display.showTare();
    scale.tare();
    dosing.start();
  }
  else if (button == Button::CLEAR) {
    // CLEAR тарирует без старта дозирования.
    display.showTare();
    scale.tare();
  }
}

void WorkScreen::updateRgb() {
  if (dosing.isDosing()) {
    rgb.dosing();
  }
  else {
    rgb.ready();
  }
}
