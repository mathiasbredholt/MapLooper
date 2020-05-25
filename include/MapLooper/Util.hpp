/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define randint() esp_random()

namespace MapLooper {
inline float lerpf32(float a, float b, float x);
void print_timestamp(int timeStamp);

template <int MAX>
int lerp8(int a, int b, int x) {
  return (a * (MAX - x) + b * x) / MAX;
}

inline int mod(int in, int hi) {
  const int lo = 0;
  if (in >= hi) {
    in -= hi;
    if (in < hi) {
      return in;
    }
  } else if (in < lo) {
    in += hi;
    if (in >= lo) {
      return in;
    }
  } else {
    return in;
  }
  if (hi == lo) {
    return lo;
  }
  int c;
  c = in % hi;
  if (c < 0) {
    c += hi;
  }
  return c;
}

inline int wrap(int in, int lo, int hi) {
  return mod(in - lo, hi - lo + 1) + lo;
}

inline int fold(int in, int lo, int hi) {
  int b = hi - lo;
  int b2 = b + b;
  int c = mod(in - lo, b2);
  if (c > b) c = b2 - c;
  return c + lo;
}

inline float fold(float in, float lo, float hi) {
  float x, c, range, range2;
  x = in - lo;

  // avoid the divide if possible
  if (in >= hi) {
    in = hi + hi - in;
    if (in >= lo) return in;
  } else if (in < lo) {
    in = lo + lo - in;
    if (in < hi) return in;
  } else
    return in;

  if (hi == lo) return lo;
  // ok do the divide
  range = hi - lo;
  range2 = range + range;
  c = x - range2 * std::floor(x / range2);
  if (c >= range) c = range2 - c;
  return c + lo;
}

inline float lerpf32(float a, float b, float x) {
  return a * (1.0 - x) + b * x;
}

template <typename T>
inline T clip(T x, T min, T max) {
  return std::min<T>(std::max<T>(x, min), max);
}

inline bool within_range(int num, int min, int max, int steps) {
  if (max < steps) {
    return num >= min && num <= max;
  } else {
    return !(num > (max % steps) && num < min);
  }
}

#ifndef ESP_PLATFORM
const std::chrono::system_clock::time_point startTime =
    std::chrono::system_clock::now();
#endif

inline uint32_t millis() {
#ifdef ESP_PLATFORM
  return esp_timer_get_time() / 1000;
#else
  return static_cast<uint32_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now() - startTime)
          .count());
#endif
}

inline void sleep(uint32_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }

inline void print_timestamp(int time_stamp) {
  int ticks = time_stamp % 96;
  int beats = ((time_stamp / 96) % 4) + 1;
  int bars = ((time_stamp / 96) / 4) + 1;
  printf("%d : %d : %d\n", bars, beats, ticks);
}

inline int divRound(const int n, const int d) {
  return (n < 0) ? ((n - d / 2) / d) : ((n + d / 2) / d);
}

class mutex {
 public:
  mutex() { m = xSemaphoreCreateMutex(); }

  void lock() { xSemaphoreTake(m, portMAX_DELAY); }

  void unlock() { xSemaphoreGive(m); }

 private:
  SemaphoreHandle_t m;
};
}  // namespace MapLooper
