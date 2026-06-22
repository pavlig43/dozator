#include <Arduino.h>
#include "dosing_controller.h"

#define SMALL_REMAINING_LIMIT_GRAMS 200
#define MEDIUM_REMAINING_LIMIT_GRAMS 400
#define MAIN_FILL_STOP_MARGIN_GRAMS 50
#define WAIT_TIME_MS 1000UL
#define SLOW_FILL_TIME_MS 1000UL

DosingController::DosingController(Servo& servo, ServoMemory& servoMemory, Scale& scale, Motor& motor, TargetMemory& targetMemory)
  : servo(servo),
    servoMemory(servoMemory),
    scale(scale),
    motor(motor),
    targetMemory(targetMemory),
    waitTimer(WAIT_TIME_MS),
    slowFillTimer(SLOW_FILL_TIME_MS) {
}

void DosingController::init() {
  servoMemory.init();
  servo.init(servoMemory.closedAngle());
  motor.init();
}

void DosingController::start() {
  // Старт всегда начинается с быстрой основной подачи.
  setState(State::MAIN_FILL);
  motor.setFilling(true);
}

void DosingController::stop() {
  // Это общий безопасный стоп для выхода с экрана и достижения цели.
  setState(State::IDLE);
  servo.setAngle(servoMemory.closedAngle());
  motor.setFilling(false);
}

bool DosingController::isDosing() const {
  return state != State::IDLE;
}

// Автомат дозирования:
// MAIN_FILL открывает заслонку по остатку веса, WAIT ждёт осадки,
// SLOW_FILL коротко досыпает через slowAngle и снова возвращается в WAIT.
void DosingController::loop() {
  const unsigned long targetWeight = targetMemory.weight();
  const long currentWeight = scale.weightGrams();

  // Как только цель достигнута, останавливаемся из любого рабочего состояния.
  if (state != State::IDLE && currentWeight >= (long)targetWeight) {
    stop();
    return;
  }

  switch (state) {
  case State::MAIN_FILL:
    // Основную подачу прекращаем заранее, потому что продукт ещё досыпается по инерции.
    if (currentWeight >= (long)targetWeight - MAIN_FILL_STOP_MARGIN_GRAMS) {
      servo.setAngle(servoMemory.closedAngle());
      motor.setFilling(false);
      setState(State::WAIT);
    }
    else {
      servo.setAngle(mainFillAngle((long)targetWeight - currentWeight));
    }
    break;

  case State::WAIT:
    // После любой подачи ждём осадки веса, затем делаем короткую медленную досыпку.
    if (waitTimer.ready()) {
      servo.setAngle(servoMemory.slowAngle());
      motor.setFilling(true);
      setState(State::SLOW_FILL);
    }
    break;

  case State::SLOW_FILL:
    // Медленная досыпка идёт фиксированное время, затем снова ждём стабилизации.
    if (slowFillTimer.ready()) {
      servo.setAngle(servoMemory.closedAngle());
      motor.setFilling(false);
      setState(State::WAIT);
    }
    break;

  case State::IDLE:
    servo.setAngle(servoMemory.closedAngle());
    motor.setFilling(false);
    break;
  }
}

void DosingController::setState(State nextState) {
  state = nextState;

  // Таймеры сбрасываются в момент входа в состояние, а не при выходе из него.
  if (nextState == State::WAIT) {
    waitTimer.reset();
  }
  else if (nextState == State::SLOW_FILL) {
    slowFillTimer.reset();
  }
}

// Чем меньше осталось до цели, тем сильнее прикрыта заслонка.
int DosingController::mainFillAngle(long remainingWeight) const {
  const int angleRange = servoMemory.closedAngle() - servoMemory.openAngle();

  if (remainingWeight < SMALL_REMAINING_LIMIT_GRAMS) {
    return servoMemory.closedAngle() - angleRange / 3;
  }

  if (remainingWeight < MEDIUM_REMAINING_LIMIT_GRAMS) {
    return servoMemory.closedAngle() - angleRange / 2;
  }

  return servoMemory.openAngle();
}
