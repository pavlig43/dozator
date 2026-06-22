#pragma once

#include "tmr.h"

class ServoControl {
public:
  enum AngleType {
    OPEN_ANGLE,
    CLOSED_ANGLE,
    SLOW_ANGLE
  };

private:
  enum DosingState {
    DOSING_IDLE, // Дозирование остановлено.
    DOSING_MAIN_FILL, // Основная быстрая подача до подхода к цели.
    DOSING_MAIN_WAIT, // Ожидание после основной подачи, пока вес стабилизируется.
    DOSING_SLOW_FILL, // Короткая медленная досыпка.
    DOSING_SLOW_WAIT // Ожидание после медленной досыпки.
  };

  int lastAngle; // Последний угол, отправленный в серво.
  DosingState dosingState; // Текущее состояние автомата дозирования.
  int openAngle; // Угол полного открытия заслонки.
  int closedAngle; // Угол закрытой заслонки.
  int slowAngle; // Угол медленной досыпки.
  AngleType activeAngle; // Угол, который сейчас меняется в режиме настройки.
  bool angleSetupActive; // Режим ручной настройки углов.
  Tmr waitTimer; // Таймер ожидания стабилизации веса.
  Tmr slowFillTimer; // Таймер длительности медленной досыпки.

  void writeAngle(int angle);
  int getMainFillAngle(long remainingWeight) const;
  void setDosingState(DosingState newState);

public:
  constexpr ServoControl()
    : lastAngle(-1),
      dosingState(DOSING_IDLE),
      openAngle(90),
      closedAngle(90),
      slowAngle(90),
      activeAngle(OPEN_ANGLE),
      angleSetupActive(false),
      waitTimer(1000UL),
      slowFillTimer(1000UL) {
  }

  void setup();
  void loop();
  void startCycle();
  void stopCycle();
  bool isOpening() const;
  void loadSettings();
  void saveSettings() const;
  int getAngle(AngleType angleType) const;
  void setActiveAngle(AngleType angleType);
  void resetSlowAngleToClosed();
  void changeActiveAngle(int delta);
  void finishAngleSetup();
};

ServoControl& servo();
