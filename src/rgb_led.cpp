#include <Arduino.h>
#include "rgb_led.h"

#define RGB_BLUE_PIN 5
#define RGB_GREEN_PIN 6
#define RGB_RED_PIN 7

void Rgb::off() {
  digitalWrite(RGB_BLUE_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, LOW);
  digitalWrite(RGB_RED_PIN, LOW);
}

void Rgb::ready() {
  digitalWrite(RGB_BLUE_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, HIGH);
  digitalWrite(RGB_RED_PIN, LOW);
}

void Rgb::dosing() {
  digitalWrite(RGB_BLUE_PIN, HIGH);
  digitalWrite(RGB_GREEN_PIN, LOW);
  digitalWrite(RGB_RED_PIN, LOW);
}

void Rgb::init() {
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  off();
}
