#include <Arduino.h>
#include "app.h"

void setup() {
  app().init();
}

void loop() {
  app().loop();
}
