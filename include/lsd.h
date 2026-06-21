#pragma once
#include <Arduino.h>
#include "servo_control.h"

void lsdSetup();
void lsdShowLoading();
void lsdShowTare();
void lsdShowDigits(const byte digits[5], byte cursor);
void lsdUpdateDigits(const byte digits[5]);
void lsdUpdateCursor(byte cursor);
void lsdShowWorkScreen(long currentWeight, long targetWeight, bool showChHint);
void lsdUpdateCurrentWeight(long currentWeight, bool showChHint);
void lsdShowAngleSetup(ServoAngleType angleType, int angle);
