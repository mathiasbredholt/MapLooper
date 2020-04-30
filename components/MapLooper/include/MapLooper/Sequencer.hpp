/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Sequencer.hpp
Class to sequence patterns

*/

#pragma once

#include <vector>
#include <functional>

#include "MapLooper/Clock.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/GestureRecorder.hpp"

namespace MapLooper {

class Sequencer {
 public:
  explicit Sequencer(Pattern* ptn);

  void start();

  void stop();

  void update();

  bool is_playing();

  void set_tempo(int val);

  void set_tempo_relative(int val);

  int get_tempo();

  tick_t get_ticks();

  const Clock& get_clock();

  GestureRecorder* get_recorder();

 private:
  void start_callback_();

  void stop_callback_();

  void active_sensing_();

  void midi_beat_clock_(int32_t t);

  int32_t last_active_sensing_{0};

  tick_t _tick;

  Pattern* _ptn;

  GestureRecorder _recorder;

  TickFunction tick_function_;

  Clock clock_;

  bool is_playing_ = false;

};

}  // namespace MapLooper