#include <Arduino.h>
#include "angle_settings.h"

AngleSettings::AngleSettings(Servo& servo, ServoMemory& memory)
  : servo(servo),
    memory(memory) {
}

void AngleSettings::init() {
  memory.init();
  servo.init(memory.closedAngle());
}

void AngleSettings::begin() {
  // Каждый вход в экран настройки начинается с открытого угла.
  moveTo(Step::OPEN);
}

void AngleSettings::loop() {
  servo.loop();
}

// Возвращает true, если экран настройки нужно перерисовать.
bool AngleSettings::handleButton(Button button) {
  if (step == Step::NONE) {
    return false;
  }

  if (button == Button::UP || button == Button::DOWN) {
    // UP/DOWN меняют текущий угол на 1 градус.
    // Ограничение 10..170 находится внутри ServoMemory::set().
    const int delta = button == Button::UP ? 1 : -1;
    const AngleKind kind = kindForStep();
    memory.set(kind, memory.get(kind) + delta);
    servo.setAngle(memory.get(kind));
    return true;
  }

  if (button != Button::NEXT) {
    return false;
  }

  if (step == Step::OPEN) {
    moveTo(Step::CLOSED);
    return true;
  }

  if (step == Step::CLOSED) {
    // По старой логике slowAngle начинается с closedAngle:
    // пользователь приоткрывает заслонку только настолько, насколько нужно для досыпки.
    memory.set(AngleKind::SLOW, memory.closedAngle());
    moveTo(Step::SLOW);
    return true;
  }

  // После slowAngle сохраняем все три угла и возвращаем заслонку в закрытое положение.
  memory.save();
  step = Step::NONE;
  servo.setAngle(memory.closedAngle());
  return true;
}

bool AngleSettings::isActive() const {
  return step != Step::NONE;
}

AngleKind AngleSettings::currentKind() const {
  return kindForStep();
}

int AngleSettings::currentAngle() const {
  return memory.get(kindForStep());
}

void AngleSettings::moveTo(Step nextStep) {
  step = nextStep;
  servo.setAngle(memory.get(kindForStep()));
}

AngleKind AngleSettings::kindForStep() const {
  if (step == Step::OPEN) {
    return AngleKind::OPEN;
  }

  if (step == Step::CLOSED) {
    return AngleKind::CLOSED;
  }

  return AngleKind::SLOW;
}
