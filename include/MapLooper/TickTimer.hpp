/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

TickTimer.hpp
Setup timer + interrupt

*/

#pragma once

#include <functional>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"

namespace MapLooper {
class TickTimer {
 public:
  TickTimer(TaskFunction_t handler, void* userParam = nullptr) {
    xTaskCreate(handler, "timerTask", 4096, userParam, 25, &_taskHandle);

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
