#pragma once

#include <Arduino.h>
#include "ir.h"
#include "lsd.h"
#include "navigation.h"
#include "target_memory.h"
#include "weight_input.h"

// Экран ввода целевого веса.
// Владеет состоянием курсора и цифр через WeightInput, а при подтверждении
// сохраняет вес в TargetMemory и просит Navigation открыть рабочий экран.
class WeightInputScreen {
public:
  WeightInputScreen(Display& display, TargetMemory& targetMemory, Navigation& navigation);
  void init(); // Ленивая загрузка сохранённого веса из EEPROM.
  void enter(); // Полностью рисует экран ввода при входе.
  void loop(); // Обновляет только изменившиеся цифры и курсор.
  void handleButton(Button button); // Обрабатывает цифры, курсор, очистку и переходы.

private:
  static constexpr byte NOT_DRAWN_YET = 255; // Невозможное значение для цифры/курсора.

  Display& display;
  TargetMemory& targetMemory;
  Navigation& navigation;
  WeightInput input;
  bool initialized = false; // Защита от повторной загрузки EEPROM при возврате на экран.
  // Кэш того, что уже нарисовано на LCD. Нужен, чтобы не писать в медленный LCD без причины.
  byte displayedDigits[5] = {NOT_DRAWN_YET, NOT_DRAWN_YET, NOT_DRAWN_YET, NOT_DRAWN_YET, NOT_DRAWN_YET};
  byte displayedCursor = NOT_DRAWN_YET;

  void rememberDisplayedDigits();
  bool digitsNeedRedraw() const;
};
