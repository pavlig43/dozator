#pragma once

#include <Arduino.h>
#include "angle_settings.h"
#include "angle_settings_screen.h"
#include "dosing_controller.h"
#include "ir.h"
#include "lsd.h"
#include "motor_driver.h"
#include "navigation.h"
#include "rgb_led.h"
#include "scale_sensor.h"
#include "servo_memory.h"
#include "servo_motor.h"
#include "target_memory.h"
#include "weight_input_screen.h"
#include "work_screen.h"

// App - тонкий host приложения.
// Он не содержит бизнес-логику экранов: только поднимает базовые устройства,
// читает кнопки с пульта и передаёт управление текущему экрану через Navigation.
class App {
public:
  App();
  void init(); // Однократный старт host-части приложения.
  void loop(); // Главный цикл: кнопка, навигация, loop активного экрана.

private:
  // Железо и общие сервисы хранятся статически, без new/delete.
  // Для Arduino Nano это предсказуемее: нет фрагментации heap при переходах экранов.
  IrRemote remote;
  Display display;
  Scale scale;
  Motor motor;
  Rgb rgb;
  Servo servo;
  ServoMemory servoMemory;
  TargetMemory targetMemory;
  Navigation navigation;

  // Фичи и экраны получают зависимости ссылками и сами решают,
  // что делать в enter(), loop(), exit() и handleButton().
  AngleSettings angleSettings;
  DosingController dosing;
  WeightInputScreen weightInputScreen;
  AngleSettingsScreen angleSettingsScreen;
  WorkScreen workScreen;
  bool menuActive = true; // При старте открыто меню с выбранным рабочим экраном.
  ScreenId selectedScreen = ScreenId::WORK; // Текущий пункт меню.

  void handleButton(Button button); // Передаёт кнопку активному экрану.
  void loopCurrentScreen(); // Вызывает loop() только у текущего экрана.
  void enterCurrentScreen(); // Вызывает enter() у текущего экрана.
  void exitCurrentScreen(); // Даёт текущему экрану остановиться перед уходом.
  void applyNavigation(); // Применяет запрошенный переход между экранами.
  void showMenu(); // Рисует меню выбора экрана.
  void handleMenuButton(Button button); // PREV/NEXT листают меню, CONFIRM открывает экран.
  void openSelectedScreen(); // Закрывает меню и открывает выбранный экран.
  void openScreen(ScreenId screen); // Открывает экран с правильным enter/exit.
  void openMenu(ScreenId screen); // Открывает меню с выбранным пунктом.
  void selectMenuByButton(Button button); // Циклически выбирает экран по PREV/NEXT.
  static ScreenId menuScreenByButton(ScreenId screen, Button button);
};

App& app();
