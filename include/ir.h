#pragma once
#include <Arduino.h>

enum class Button : byte {
  NONE,    // Нет кнопки или неизвестный IR-код.
  PREV,    // Предыдущий пункт меню.
  NEXT,    // Переход к следующему шагу/экрану.
  START,   // Запуск дозирования на рабочем экране.
  UP,      // Увеличить угол.
  DOWN,    // Уменьшить угол.
  CLEAR,   // Очистка ввода или тарирование.
  CONFIRM, // Подтверждение/выход, зависит от текущего экрана.
  DIGIT_0,
  DIGIT_1,
  DIGIT_2,
  DIGIT_3,
  DIGIT_4,
  DIGIT_5,
  DIGIT_6,
  DIGIT_7,
  DIGIT_8,
  DIGIT_9
};

// IrRemote знает только сырые коды пульта и переводит их в понятные Button.
// Он не меняет экраны, не пишет EEPROM и не управляет железом.
class IrRemote {
public:
  void init(); // Запускает IRremote на нужном пине.
  Button read(); // Возвращает одну понятную кнопку или NONE.
  static int digit(Button button); // Для DIGIT_0..DIGIT_9 возвращает 0..9, иначе -1.

private:
  Button decode(uint32_t rawCode) const;
};
