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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"

namespace MapLooper {
class TickTimer {
 public:
  TickTimer(TaskFunction_t handler, void* userParam = nullptr) {
    xTaskCreatePinnedToCore(handler, "timerTask", 4096, userParam, 40, &_taskHandle, 1);

    timer_config_t config = {.alarm_en = TIMER_ALARM_EN,
                             .counter_en = TIMER_PAUSE,
                             .intr_type = TIMER_INTR_LEVEL,
                             .counter_dir = TIMER_COUNT_UP,
                             .auto_reload = TIMER_AUTORELOAD_EN,
                             .divider = 80};

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TICK_TIMER_PERIOD);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_group0_isr, _taskHandle, 0,
                       nullptr);

    timer_start(TIMER_GROUP_0, TIMER_0);
  }

 private:
    const static int TICK_TIMER_PERIOD = 1000;
    TaskHandle_t _taskHandle;

  static void timer_group0_isr(void* userParam);
};

}  // namespace MapLooper
