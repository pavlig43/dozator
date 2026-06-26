#include <Arduino.h>
#include <IRremote.hpp>
#include "ir.h"

#define IR_RECEIVE_PIN 8
#define START_BUTTON 0xB946FF00UL // CH
#define EQ_BUTTON 0xF609FF00UL // EQ
#define PLAY_BUTTON 0xBC43FF00UL // PLAY/PAUSE
#define PREV_BUTTON 0xBB44FF00UL // PREV
#define NEXT_BUTTON 0xBF40FF00UL // NEXT
#define VOLUME_DOWN_BUTTON 0xF807FF00UL // VOL-
#define VOLUME_UP_BUTTON 0xEA15FF00UL // VOL+

#define ZERO_BUTTON 0xE916FF00UL
#define ONE_BUTTON 0xF30CFF00UL
#define TWO_BUTTON 0xE718FF00UL
#define THREE_BUTTON 0xA15EFF00UL
#define FOUR_BUTTON 0xF708FF00UL
#define FIVE_BUTTON 0xE31CFF00UL
#define SIX_BUTTON 0xA55AFF00UL
#define SEVEN_BUTTON 0xBD42FF00UL
#define EIGHT_BUTTON 0xAD52FF00UL
#define NINE_BUTTON 0xB54AFF00UL

// Запускает приёмник ИК-пульта. Вся логика экранов и режимов находится в App.
void IrRemote::init() {
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// Читает одну кнопку. Если сигнала нет или код неизвестен, возвращает NONE.
Button IrRemote::read() {
  if (!IrReceiver.decode()) {
    return Button::NONE;
  }

  const uint32_t rawCode = IrReceiver.decodedIRData.decodedRawData;
  IrReceiver.resume();
  return decode(rawCode);
}

Button IrRemote::decode(uint32_t rawCode) const {
  // Здесь единственное место, где сырые HEX-коды пульта превращаются в смысловые кнопки.
  switch (rawCode) {
  case PREV_BUTTON: return Button::PREV;
  case NEXT_BUTTON: return Button::NEXT;
  case START_BUTTON: return Button::START;
  case VOLUME_UP_BUTTON: return Button::UP;
  case VOLUME_DOWN_BUTTON: return Button::DOWN;
  case EQ_BUTTON: return Button::CLEAR;
  case PLAY_BUTTON: return Button::CONFIRM;
  case ZERO_BUTTON: return Button::DIGIT_0;
  case ONE_BUTTON: return Button::DIGIT_1;
  case TWO_BUTTON: return Button::DIGIT_2;
  case THREE_BUTTON: return Button::DIGIT_3;
  case FOUR_BUTTON: return Button::DIGIT_4;
  case FIVE_BUTTON: return Button::DIGIT_5;
  case SIX_BUTTON: return Button::DIGIT_6;
  case SEVEN_BUTTON: return Button::DIGIT_7;
  case EIGHT_BUTTON: return Button::DIGIT_8;
  case NINE_BUTTON: return Button::DIGIT_9;
  default: return Button::NONE;
  }
}

int IrRemote::digit(Button button) {
  // Значения DIGIT_0..DIGIT_9 идут подряд в enum, поэтому цифру можно посчитать.
  if (button >= Button::DIGIT_0 && button <= Button::DIGIT_9) {
    return static_cast<int>(button) - static_cast<int>(Button::DIGIT_0);
  }

  return -1;
}
