#pragma once

// Простой неблокирующий таймер на millis().
// Используется вместо delay(), чтобы основной loop() продолжал читать кнопки и весы.
class Tmr {
    unsigned long _period; // Период ожидания в миллисекундах.
    unsigned long lastTime; // millis() последнего reset() или срабатывания ready().

public:
    explicit constexpr Tmr(unsigned long period)
        : _period(period),
          lastTime(0) {
    }

    void reset(); // Начать отсчёт периода заново.
    bool ready(); // true один раз за период, затем автоматически перезапускает отсчёт.
};
