#include <Arduino.h>
#include <HX711.h>
#include "scale_sensor.h"

#define SCALE_DT_PIN 3
#define SCALE_SCK_PIN 2
#define SCALE_FACTOR 407.5
#define SCALE_AVERAGE_SAMPLES 5

HX711 scale;

// Последнее сырое показание HX711. Используется как нулевая точка при тарировании.
long lastRawValue = 0;

// Последний усреднённый вес в граммах для автомата дозирования и экрана.
long lastWeightGrams = 0;

// Кольцевой буфер последних измерений веса для скользящего среднего.
long weightSamples[SCALE_AVERAGE_SAMPLES] = {};

// Сумма значений в кольцевом буфере для быстрого расчёта среднего.
long weightSamplesSum = 0;

// Количество заполненных элементов буфера до накопления пяти измерений.
byte weightSampleCount = 0;

// Позиция в буфере, куда будет записано следующее измерение.
byte weightSampleIndex = 0;

// Показывает, было ли получено хотя бы одно показание от HX711.
bool hasRawValue = false;

// Требует принять ближайшее готовое показание HX711 за нулевой вес.
bool tareRequested = true;

// Очищает накопленные показания, чтобы усреднение началось заново.
void resetWeightAverage() {
  weightSamplesSum = 0;
  weightSampleCount = 0;
  weightSampleIndex = 0;
}

// Добавляет новое измерение в скользящее окно и сохраняет средний вес.
void addWeightSample(long weightGrams) {
  if (weightSampleCount < SCALE_AVERAGE_SAMPLES) {
    weightSampleCount++;
  }
  else {
    weightSamplesSum -= weightSamples[weightSampleIndex];
  }

  weightSamples[weightSampleIndex] = weightGrams;
  weightSamplesSum += weightGrams;
  weightSampleIndex = (weightSampleIndex + 1) % SCALE_AVERAGE_SAMPLES;
  lastWeightGrams = weightSamplesSum / weightSampleCount;
}

// Подключает HX711 и задаёт калибровочный коэффициент.
// Первое готовое показание будет принято за ноль без блокировки программы.
void Scale::init() {
  scale.begin(SCALE_DT_PIN, SCALE_SCK_PIN);
  scale.set_scale(SCALE_FACTOR);
}

// Если HX711 подготовил данные, читает одно показание и обновляет средний вес.
// Здесь Tmr не нужен: HX711 сам сообщает готовность через is_ready().
// Если читать по таймеру, можно либо пропустить готовое измерение, либо ждать лишнее время.
// Поэтому loop() просто часто спрашивает датчик и сразу возвращается, если данных нет.
void Scale::loop() {
  if (!scale.is_ready()) {
    return;
  }

  lastRawValue = scale.read();
  hasRawValue = true;

  if (tareRequested) {
    scale.set_offset(lastRawValue);
    lastWeightGrams = 0;
    resetWeightAverage();
    tareRequested = false;
    return;
  }

  const float rawDelta = static_cast<float>(lastRawValue - scale.get_offset());
  const float weight = rawDelta / scale.get_scale();
  long roundedWeight = (long)(weight + (weight >= 0.0f ? 0.5f : -0.5f));
  addWeightSample(roundedWeight);
}

// Следующее готовое показание HX711 станет нулём.
void Scale::requestTare() {
  tareRequested = true;
}

bool Scale::tareDone() const {
  return !tareRequested;
}

// Возвращает сохранённый средний вес без нового обращения к HX711.
// Это значение одновременно используют автомат дозирования и экран.
long Scale::weightGrams() const {
  return lastWeightGrams;
}
