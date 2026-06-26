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

  if (tareAction != TareAction::NONE) {
    servo.loop();
    updateRgb();

    if (scale.tareDone()) {
      finishTare();
    }

    return;
  }

  dosing.loop();
  servo.loop();
  updateRgb();

  // LCD обновляем реже, чем читаем весы и крутим автомат, потому что запись в LCD медленная.
  if (weightScreenTimer.ready()) {
    display.updateCurrentWeight(scale.weightGrams(), !dosing.isDosing());
  }
}

void WorkScreen::handleButton(Button button) {
  if (button == Button::START) {
    beginTare(TareAction::START_AFTER_TARE);
  }
  else if (button == Button::CLEAR) {
    beginTare(TareAction::TARE_ONLY);
  }
}

void WorkScreen::beginTare(TareAction action) {
  dosing.stop();
  display.showTare();
  scale.requestTare();
  tareAction = action;
}

void WorkScreen::finishTare() {
  const TareAction completedAction = tareAction;
  tareAction = TareAction::NONE;

  if (completedAction == TareAction::START_AFTER_TARE) {
    dosing.start();
  }

  display.showWork(scale.weightGrams(), targetMemory.weight(), !dosing.isDosing());
  weightScreenTimer.reset();
}

void WorkScreen::updateRgb() {
  if (dosing.isDosing()) {
    rgb.dosing();
  }
  else {
    rgb.ready();
  }
}
