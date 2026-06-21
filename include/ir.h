#pragma once
#include <Arduino.h>

extern byte digits[5];
extern byte cursor;
extern bool workScreen;

void irSetup();
void irLoop();
long irGetTargetNumber();
bool irIsAngleSetupActive();
