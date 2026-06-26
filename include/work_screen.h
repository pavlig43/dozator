#pragma once

#include <Arduino.h>
#include "dosing_controller.h"
#include "ir.h"
#include "lsd.h"
#include "navigation.h"
#include "rgb_led.h"
#include "scale_sensor.h"
#include "servo_motor.h"
#include "target_memory.h"
#include "tmr.h"

// Рабочий экран дозирования.
// Здесь находятся весы, дозирование, RGB-индикация и тарирование,
// потому что всё это используется только во время работы.
class WorkScreen {
public:
  WorkScreen(Display& display, Scale& scale, Rgb& rgb, DosingController& dosing, TargetMemory& targetMemory, Navigation& navigation, Servo& servo);
  void enter(); // Рисует рабочий экран и включает рабочую индикацию.
  void exit(); // Останавливает дозирование и гасит RGB перед уходом.
  void loop(); // Читает вес, двигает автомат дозирования, обновляет LCD/RGB.
  void handleButton(Button button); // START запускает цикл, CLEAR тарирует.

private:
  enum class TareAction : byte {
    NONE,
    TARE_ONLY,
    START_AFTER_TARE
  };

  Display& display;
  Scale& scale;
  Rgb& rgb;
  DosingController& dosing;
  TargetMemory& targetMemory;
  Navigation& navigation;
  Servo& servo;
  bool initialized = false; // Весы, RGB, мотор и серво поднимаются при первом входе.
  TareAction tareAction = TareAction::NONE; // Что сделать после свежего нуля весов.
  Tmr weightScreenTimer; // Периодическое обновление веса на LCD без delay().

  void init(); // Ленивая инициализация железа, которое нужно только рабочему экрану.
  void beginTare(TareAction action); // Запрашивает свежую тару и ждёт её в loop().
  void finishTare(); // Возвращает экран к работе после завершения тарирования.
  void updateRgb(); // Зелёный - готово, синий - идёт дозирование.
};
