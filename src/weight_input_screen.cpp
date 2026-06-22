#include "weight_input_screen.h"

WeightInputScreen::WeightInputScreen(Display& display, TargetMemory& targetMemory, Navigation& navigation)
  : display(display),
    targetMemory(targetMemory),
    navigation(navigation) {
}

void WeightInputScreen::init() {
  // EEPROM читаем один раз. После этого пользователь может менять цифры на экране,
  // и повторный enter() не должен внезапно вернуть старое сохранённое значение.
  if (initialized) {
    return;
  }

  targetMemory.init();
  input.setTargetWeight(targetMemory.weight());
  initialized = true;
}

void WeightInputScreen::enter() {
  init();
  // При входе рисуем весь экран, а дальше loop() обновляет только изменённые части.
  display.showWeightInput(input.getDigits(), input.getCursor());
  rememberDisplayedDigits();
  displayedCursor = input.getCursor();
}

void WeightInputScreen::loop() {
  if (digitsNeedRedraw()) {
    display.updateDigits(input.getDigits());
    rememberDisplayedDigits();
  }

  if (input.getCursor() != displayedCursor) {
    display.updateCursor(input.getCursor());
    displayedCursor = input.getCursor();
  }
}

void WeightInputScreen::handleButton(Button button) {
  if (button == Button::NEXT) {
    // NEXT не меняет вес, а открывает отдельный сценарий настройки углов.
    navigation.openAngleSettings();
    return;
  }

  if (button == Button::CONFIRM) {
    // Нулевой вес не сохраняем: рабочий режим без цели не имеет смысла.
    const unsigned long targetWeight = input.targetWeight();
    if (targetWeight > 0) {
      targetMemory.save(targetWeight);
      navigation.openWork();
    }
    return;
  }

  // Все обычные кнопки редактирования остаются внутри WeightInput.
  input.handleButton(button);
}

void WeightInputScreen::rememberDisplayedDigits() {
  // Запоминаем то, что уже отправили на LCD, для точечного обновления.
  const byte* digits = input.getDigits();

  for (byte i = 0; i < 5; i++) {
    displayedDigits[i] = digits[i];
  }
}

bool WeightInputScreen::digitsNeedRedraw() const {
  // LCD медленный, поэтому сравниваем цифры и не пишем в него без необходимости.
  // NOT_DRAWN_YET - невозможная цифра, она означает "кэш ещё не заполнен".
  const byte* digits = input.getDigits();

  for (byte i = 0; i < 5; i++) {
    if (displayedDigits[i] == NOT_DRAWN_YET || digits[i] != displayedDigits[i]) {
      return true;
    }
  }

  return false;
}
