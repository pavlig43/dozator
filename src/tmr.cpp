#include <Arduino.h>
#include "tmr.h"

Tmr::Tmr(unsigned long period) {
    _period = period;
    lastTime = 0;
}

bool Tmr::ready() {
    unsigned long now = millis();

    if (now - lastTime >= _period) {
        lastTime = now;
        return true;
    }

    return false;
}
