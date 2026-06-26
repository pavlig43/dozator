#include <Arduino.h>
#include "angle_settings.h"

namespace {
const AngleSettingKind SETTING_ORDER[] = {
  AngleSettingKind::OPEN_ANGLE,
  AngleSettingKind::CLOSED_ANGLE,
  AngleSettingKind::SLOW_ANGLE,
  AngleSettingKind::STOP_WEIGHT
};

const byte SETTING_COUNT = sizeof(SETTING_ORDER) / sizeof(SETTING_ORDER[0]);
}

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
  moveToIndex(0);
}

void AngleSettings::end() {
  memory.save();
  active = false;
  servo.setAngle(memory.closedAngle());
}

void AngleSettings::loop() {
  servo.loop();
}

// Возвращает true, если экран настройки нужно перерисовать.
bool AngleSettings::handleButton(Button button) {
  if (!isActive()) {
    return false;
  }

  if (button == Button::UP || button == Button::DOWN) {
    // UP/DOWN меняют текущий угол на 1 градус или вес на 1 грамм.
    const int delta = button == Button::UP ? 1 : -1;
    if (currentSettingIsAngle()) {
      const AngleKind kind = currentAngleKind();
      memory.set(kind, memory.get(kind) + delta);
      servo.setAngle(memory.get(kind));
    }
    else {
      memory.setStopMarginGrams((int)memory.stopMarginGrams() + delta);
    }
    return true;
  }

  if (button == Button::PREV || button == Button::NEXT) {
    selectByButton(button);
    return true;
  }

  return false;
}

bool AngleSettings::isActive() const {
  return active;
}

AngleSettingKind AngleSettings::currentKind() const {
  return currentSettingKind();
}

int AngleSettings::currentValue() const {
  if (currentSettingIsAngle()) {
    return memory.get(currentAngleKind());
  }

  return memory.stopMarginGrams();
}

void AngleSettings::moveToIndex(byte nextIndex) {
  active = true;
  currentIndex = nextIndex;
  if (currentSettingIsAngle()) {
    servo.setAngle(memory.get(currentAngleKind()));
  }
}

void AngleSettings::selectByButton(Button button) {
  if (button == Button::PREV) {
    currentIndex = (currentIndex + SETTING_COUNT - 1) % SETTING_COUNT;
  }
  else {
    currentIndex = (currentIndex + 1) % SETTING_COUNT;
  }

  if (currentSettingIsAngle()) {
    servo.setAngle(memory.get(currentAngleKind()));
  }
}

AngleSettingKind AngleSettings::currentSettingKind() const {
  return SETTING_ORDER[currentIndex];
}

bool AngleSettings::currentSettingIsAngle() const {
  return currentSettingKind() != AngleSettingKind::STOP_WEIGHT;
}

AngleKind AngleSettings::currentAngleKind() const {
  const AngleSettingKind kind = currentSettingKind();

  if (kind == AngleSettingKind::OPEN_ANGLE) {
    return AngleKind::OPEN;
  }

  if (kind == AngleSettingKind::CLOSED_ANGLE) {
    return AngleKind::CLOSED;
  }

  return AngleKind::SLOW;
}
