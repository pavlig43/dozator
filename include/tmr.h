#pragma once

class Tmr {
    unsigned long _period;
    unsigned long lastTime;

public:
    explicit constexpr Tmr(unsigned long period)
        : _period(period),
          lastTime(0) {
    }

    void reset();
    bool ready();
};
