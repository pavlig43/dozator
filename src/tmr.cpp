#include <Arduino.h>
#include "tmr.h"

void Tmr::reset() {
    lastTime = millis();
}

bool Tmr::ready() {
    const unsigned long now = millis();

    if (now - lastTime >= _period) {
        lastTime = now;
        return true;
    }

    return false;
}
