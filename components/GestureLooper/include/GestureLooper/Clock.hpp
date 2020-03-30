/*

          ___  
         (   ) 
  .--.    | |  
 /    \   | |  
;  ,-. '  | |  
| |  | |  | |  
| |  | |  | |  
| |  | |  | |  
| '  | |  | |  
'  `-' |  | |  
 `.__. | (___) 
 ( `-' ;       
  `.__.        

Gesture Looper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Clock.hpp
A clock module that interfaces with Ableton Link.

*/

#pragma once

#include <cstdint>
#include <climits>
#include <functional>
#include <thread>

#ifndef ESP_PLATFORM
#define LINK_PLATFORM_MACOSX
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

#include "ableton/Link.hpp"

namespace GestureLooper {

class Clock {
 public:
  static constexpr double TICKS_PER_QUARTER_NOTE = 96.0;
  static constexpr double PPQN_BEAT_CLOCK = 24.0;
  static constexpr int TICKS_PER_BAR = TICKS_PER_QUARTER_NOTE * 4;
  static constexpr float DEFAULT_TEMPO = 120.0f;
  static constexpr double q = 4.0;

  Clock();
  
  void reset();

  void start();

  void stop();

  int32_t get_ticks() const;

  float get_tempo() const;

  void set_tempo(float val);

  bool is_linked() const;

  void set_start_stop_callback(std::function<void(bool)> callback);

private:
  ableton::Link _link;

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

}  // namespace GestureLooper
