#pragma once

#include <Arduino.h>
#include "ir.h"
#include "lsd.h"
#include "scale_sensor.h"
#include "tmr.h"

// Экран калибровки весов по грузу 1 кг.
class ScaleCalibrationScreen {
public:
  ScaleCalibrationScreen(Display& display, Scale& scale);
  void enter(); // Показывает первый шаг: нажать EQ для нуля.
  void loop(); // Ведёт сценарий: ноль, груз 1 кг, сохранение.
  void handleButton(Button button); // EQ запускает обнуление.

private:
  enum class Step : byte {
    WAIT_TARE,
    TARING,
    WAIT_WEIGHT,
    DONE
  };

  static const long CALIBRATION_WEIGHT_GRAMS = 1000L;
  static const byte STABLE_SAMPLE_COUNT = 5;
  static const long STABLE_RAW_TOLERANCE = 120L;
  static const long MIN_RAW_DELTA = 1000L;

  Display& display;
  Scale& scale;
  Step step = Step::WAIT_TARE;
  bool initialized = false;
  long stableRaw = 0;
  byte stableCount = 0;
  Tmr screenTimer;

  void init(); // Ленивая инициализация HX711.
  void beginTare(); // Запускает ноль.
  void finishTare(); // Просит поставить 1 кг.
  void readCalibrationWeight(); // Ждёт ровные показания и сохраняет коэф.
  void resetStableCheck(long rawDelta); // Начинает новый набор стабильных проб.
};
