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

#include <climits>
#include <cstdint>
#include <functional>

#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace MapLooper {
class Clock {
 public:
  static constexpr double TICKS_PER_QUARTER_NOTE = 48.0;
  static constexpr float DEFAULT_TEMPO = 120.0f;
  static const int q = 4;

  Clock();

  void reset();

  void start();

  void stop();

  int32_t getTicks() const;

  float getTempo() const;

  void setTempo(float val);

  bool isLinked() const;

  void setStartStopCallback(std::function<void(bool)> callback);

  int getTickInterval() const;

 private:
  void* _link;

  TaskHandle_t _link_task_handle;
};

class TickFunction {
 public:
  bool is_on_tick(int32_t ticks) {
    if (ticks != last_time) {
      last_time = ticks;
      return true;
    }
    return false;
  }

 private:
  int32_t last_time = INT_MIN;
};

}  // namespace MapLooper
