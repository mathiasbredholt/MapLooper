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

GestureRecorder.hpp
Class to record gestures

*/

#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include "GestureLooper/MIDI.hpp"
#include "GestureLooper/Pattern.hpp"
#include "GestureLooper/Util.hpp"
#include "GestureLooper/GestureVoice.hpp"

namespace GestureLooper {
class GestureRecorder {
 public:
  GestureRecorder(Pattern* ptn);

  void record(tick_t tick);

  void set_is_recording(bool enable);

  void set_pitch(int val);

  void set_pressure(int val);

  void set_timbre(int val);

  void set_pitch_mod(float val);

  void set_pressure_mod(float val);

  void set_timbre_mod(float val);

  void set_modulation(float val);

 private:
  mutex _mtx;

  Pattern* _ptn;

  bool _is_recording{false};

  gesture_frame_t _frame;

  float rand_modulation_pitch_{0};

  float rand_modulation_pressure_{0};

  float rand_modulation_timbre_{0};

  float rand_modulation_{0};
};
}  // namespace GestureLooper
