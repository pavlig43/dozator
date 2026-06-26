#include "scale_calibration_screen.h"

#define CALIBRATION_SCREEN_UPDATE_MS 200UL

ScaleCalibrationScreen::ScaleCalibrationScreen(Display& display, Scale& scale)
  : display(display),
    scale(scale),
    screenTimer(CALIBRATION_SCREEN_UPDATE_MS) {
}

void ScaleCalibrationScreen::init() {
  if (initialized) {
    return;
  }

  scale.init();
  initialized = true;
}

void ScaleCalibrationScreen::enter() {
  init();
  step = Step::WAIT_TARE;
  stableRaw = 0;
  stableCount = 0;
  display.showScaleCalibrationEq();
  screenTimer.reset();
}

void ScaleCalibrationScreen::loop() {
  scale.loop();

  if (step == Step::TARING && scale.tareDone()) {
    finishTare();
    return;
  }

  if (step == Step::WAIT_WEIGHT && screenTimer.ready()) {
    readCalibrationWeight();
  }
}

void ScaleCalibrationScreen::handleButton(Button button) {
  if (button == Button::CLEAR && step == Step::WAIT_TARE) {
    beginTare();
  }
}

void ScaleCalibrationScreen::beginTare() {
  display.showTare();
  scale.requestTare();
  step = Step::TARING;
}

void ScaleCalibrationScreen::finishTare() {
  step = Step::WAIT_WEIGHT;
  stableRaw = 0;
  stableCount = 0;
  display.showScaleCalibrationWeight();
  screenTimer.reset();
}

void ScaleCalibrationScreen::readCalibrationWeight() {
  const long rawDelta = scale.rawDelta();

  if (abs(rawDelta) < MIN_RAW_DELTA) {
    stableRaw = 0;
    stableCount = 0;
    return;
  }

  if (stableCount == 0 || abs(rawDelta - stableRaw) > STABLE_RAW_TOLERANCE) {
    resetStableCheck(rawDelta);
    return;
  }

  stableRaw = (stableRaw + rawDelta) / 2;
  stableCount++;

  if (stableCount < STABLE_SAMPLE_COUNT) {
    return;
  }

  scale.calibrateWithKnownWeight(CALIBRATION_WEIGHT_GRAMS);
  step = Step::DONE;
  display.showScaleCalibrationDone();
}

void ScaleCalibrationScreen::resetStableCheck(long rawDelta) {
  stableRaw = rawDelta;
  stableCount = 1;
}
