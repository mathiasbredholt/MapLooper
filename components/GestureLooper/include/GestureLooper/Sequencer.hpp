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

Sequencer.hpp
Class to sequence patterns

*/

#pragma once

#include <vector>
#include <functional>

#include "GestureLooper/Clock.hpp"
#include "GestureLooper/Pattern.hpp"

namespace GestureLooper {

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

  int32_t get_ticks();

  const Clock& get_clock();

  std::function<void()> on_next_ptn_in_chain;

 private:
  void start_callback_();

  void stop_callback_();

  void active_sensing_();

  void midi_beat_clock_(int32_t t);

  int32_t last_active_sensing_{0};

  int32_t ticks_;

  Pattern* ptn_;

  TickFunction tick_function_;

  Clock clock_;

  bool is_playing_ = false;

};

}  // namespace GestureLooper
