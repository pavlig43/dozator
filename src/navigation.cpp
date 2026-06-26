#include "navigation.h"

void Navigation::open(ScreenId screen) {
  requestedScreen = screen;
}

void Navigation::openWeightInput() {
  // Только запрашиваем переход. App сам выполнит exit старого и enter нового экрана.
  open(ScreenId::WEIGHT_INPUT);
}

void Navigation::openAngleSettings() {
  // Настройка углов открывается через меню выбора экрана.
  open(ScreenId::ANGLE_SETTINGS);
}

void Navigation::openScaleCalibration() {
  open(ScreenId::SCALE_CALIBRATION);
}

void Navigation::openWork() {
  // Рабочий экран открывается только после сохранения ненулевого целевого веса.
  open(ScreenId::WORK);
}

ScreenId Navigation::current() const {
  return currentScreen;
}

bool Navigation::hasPendingChange() const {
  return requestedScreen != currentScreen;
}

void Navigation::commit() {
  currentScreen = requestedScreen;
}
