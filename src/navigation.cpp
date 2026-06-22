#include "navigation.h"

void Navigation::openWeightInput() {
  // Только запрашиваем переход. App сам выполнит exit старого и enter нового экрана.
  requestedScreen = ScreenId::WEIGHT_INPUT;
}

void Navigation::openAngleSettings() {
  // Настройка углов открывается из экрана ввода веса по кнопке NEXT.
  requestedScreen = ScreenId::ANGLE_SETTINGS;
}

void Navigation::openWork() {
  // Рабочий экран открывается только после сохранения ненулевого целевого веса.
  requestedScreen = ScreenId::WORK;
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
