#include <Arduino.h>
#include <EEPROM.h>
#include <IRremote.hpp>
#include <string.h>
#include "ir.h"
#include "lsd.h"
#include "scale_sensor.h"
#include "servo_control.h"

#define IR_RECEIVE_PIN 8
#define CURSOR_LEFT_BUTTON 0xBA45FF00UL // CH-
#define START_BUTTON 0xB946FF00UL // CH
#define CURSOR_RIGHT_BUTTON 0xB847FF00UL // CH+
#define EQ_BUTTON 0xF609FF00UL // EQ
#define PLAY_BUTTON 0xBC43FF00UL // PLAY/PAUSE
#define PREV_BUTTON 0xBB44FF00UL // PREV
#define VOLUME_DOWN_BUTTON 0xF807FF00UL // VOL-
#define VOLUME_UP_BUTTON 0xEA15FF00UL // VOL+

#define EEPROM_WEIGHT_MARKER_ADDRESS 10
#define EEPROM_WEIGHT_ADDRESS 11
#define EEPROM_WEIGHT_MARKER 0x5A

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

byte digits[5] = {0, 0, 0, 5, 0};
byte cursor = 0;
bool workScreen = false;

enum AngleSetupStep {
  ANGLE_SETUP_NONE,
  ANGLE_SETUP_OPEN,
  ANGLE_SETUP_CLOSED,
  ANGLE_SETUP_SLOW
};

AngleSetupStep angleSetupStep = ANGLE_SETUP_NONE;

bool irIsAngleSetupActive() {
  return angleSetupStep != ANGLE_SETUP_NONE;
}

// Разбирает целевой вес обратно на пять цифр экрана.
void setTargetNumber(unsigned long targetNumber) {
  for (int i = 4; i >= 0; i--) {
    digits[i] = targetNumber % 10;
    targetNumber /= 10;
  }
}

// Загружает ранее подтверждённый вес, если запись в EEPROM корректна.
void loadTargetNumber() {
  if (EEPROM.read(EEPROM_WEIGHT_MARKER_ADDRESS) != EEPROM_WEIGHT_MARKER) {
    return;
  }

  unsigned long targetNumber = 0;
  EEPROM.get(EEPROM_WEIGHT_ADDRESS, targetNumber);
  setTargetNumber(targetNumber);
}

// Сохраняет подтверждённый вес без перезаписи неизменившихся байтов EEPROM.
void saveTargetNumber() {
  unsigned long targetNumber = irGetTargetNumber();
  EEPROM.put(EEPROM_WEIGHT_ADDRESS, targetNumber);
  EEPROM.update(EEPROM_WEIGHT_MARKER_ADDRESS, EEPROM_WEIGHT_MARKER);
}

// Собирает введённые цифры в одно целое число.
long irGetTargetNumber() {
  long targetNumber = 0;
  for (int i = 0; i < 5; i++) {
    targetNumber = targetNumber * 10 + digits[i];
  }
  return targetNumber;
}

// Записывает цифру с пульта в текущую позицию курсора.
void setDigit(uint32_t buttonCode) {
  if (buttonCode == ZERO_BUTTON) {
    digits[cursor] = 0;
  }
  else if (buttonCode == ONE_BUTTON) {
    digits[cursor] = 1;
  }
  else if (buttonCode == TWO_BUTTON) {
    digits[cursor] = 2;
  }
  else if (buttonCode == THREE_BUTTON) {
    digits[cursor] = 3;
  }
  else if (buttonCode == FOUR_BUTTON) {
    digits[cursor] = 4;
  }
  else if (buttonCode == FIVE_BUTTON) {
    digits[cursor] = 5;
  }
  else if (buttonCode == SIX_BUTTON) {
    digits[cursor] = 6;
  }
  else if (buttonCode == SEVEN_BUTTON) {
    digits[cursor] = 7;
  }
  else if (buttonCode == EIGHT_BUTTON) {
    digits[cursor] = 8;
  }
  else if (buttonCode == NINE_BUTTON) {
    digits[cursor] = 9;
  }
}

// Обрабатывает кнопки в режиме редактирования веса.
void handleEditButtons(uint32_t buttonCode) {
  if (buttonCode == PREV_BUTTON) {
    angleSetupStep = ANGLE_SETUP_OPEN;
    servo().setActiveAngle(ServoControl::OPEN_ANGLE);
    lsdShowAngleSetup(ServoControl::OPEN_ANGLE, servo().getAngle(ServoControl::OPEN_ANGLE));
  }
  else if (buttonCode == CURSOR_LEFT_BUTTON) {
    cursor = (cursor + 4) % 5;
  }
  else if (buttonCode == CURSOR_RIGHT_BUTTON) {
    cursor = (cursor + 1) % 5;
  }
  else if (buttonCode == EQ_BUTTON) {
    memset(digits, 0, sizeof(digits));
    cursor = 0;
  }
  else if (buttonCode == PLAY_BUTTON) {
    if (irGetTargetNumber() > 0) {
      saveTargetNumber();
      workScreen = true;
    }
  }
  else {
    setDigit(buttonCode);
  }
}

// Обрабатывает пошаговую настройку открытого, закрытого углов и досыпки.
void handleAngleSetupButtons(uint32_t buttonCode) {
  if (buttonCode == VOLUME_DOWN_BUTTON) {
    servo().changeActiveAngle(-1);
  }
  else if (buttonCode == VOLUME_UP_BUTTON) {
    servo().changeActiveAngle(1);
  }
  else if (buttonCode == PREV_BUTTON) {
    if (angleSetupStep == ANGLE_SETUP_OPEN) {
      angleSetupStep = ANGLE_SETUP_CLOSED;
      servo().setActiveAngle(ServoControl::CLOSED_ANGLE);
    }
    else if (angleSetupStep == ANGLE_SETUP_CLOSED) {
      angleSetupStep = ANGLE_SETUP_SLOW;
      servo().resetSlowAngleToClosed();
      servo().setActiveAngle(ServoControl::SLOW_ANGLE);
    }
    else {
      servo().saveSettings();
      servo().finishAngleSetup();
      angleSetupStep = ANGLE_SETUP_NONE;
      lsdShowDigits(digits, cursor);
      return;
    }
  }
  else {
    return;
  }

  ServoControl::AngleType angleType = ServoControl::SLOW_ANGLE;
  if (angleSetupStep == ANGLE_SETUP_OPEN) {
    angleType = ServoControl::OPEN_ANGLE;
  }
  else if (angleSetupStep == ANGLE_SETUP_CLOSED) {
    angleType = ServoControl::CLOSED_ANGLE;
  }
  lsdShowAngleSetup(angleType, servo().getAngle(angleType));
}

// Обрабатывает кнопки в рабочем режиме.
void handleWorkButtons(uint32_t buttonCode) {
  if (buttonCode == PLAY_BUTTON) {
    workScreen = false;
    servo().stopCycle();
    lsdShowDigits(digits, cursor);
  }
  else if (buttonCode == START_BUTTON) {
    lsdShowTare();
    scaleTare();
    servo().startCycle();
  }
  else if (buttonCode == EQ_BUTTON) {
    lsdShowTare();
    scaleTare();
  }
}

// Выбирает обработчик кнопки по текущему режиму экрана.
void handleIrButton(uint32_t buttonCode) {
  if (irIsAngleSetupActive()) {
    handleAngleSetupButtons(buttonCode);
  }
  else if (workScreen) {
    handleWorkButtons(buttonCode);
  }
  else {
    handleEditButtons(buttonCode);
  }
}

// Запускает приёмник ИК-пульта и последовательный порт.
void irSetup() {
  loadTargetNumber();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

// Читает коды с пульта и передаёт их в обработчик.
void irLoop() {
  if (IrReceiver.decode()) {
    uint32_t rawCode = IrReceiver.decodedIRData.decodedRawData;
    handleIrButton(rawCode);
    IrReceiver.resume();
  }
}
