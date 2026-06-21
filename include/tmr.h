#pragma once

class Tmr {
    unsigned long _period;
    unsigned long lastTime;

public:
    explicit Tmr(unsigned long period);
    bool ready();
};
