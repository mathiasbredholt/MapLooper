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

GestureRecorder.cpp
Class to record gestures

*/

#include "GestureLooper/GestureRecorder.hpp"

namespace GestureLooper {
GestureRecorder::GestureRecorder(Pattern* ptn) : _ptn(ptn) {}

void GestureRecorder::set_is_recording(bool enable) { _is_recording = enable; }

// void GestureRecorder::play(int channel, int32_t t, int32_t duration) {
//   // GestureVoice voice(channel, duration, t);
//   // voices_.push_back(voice);
// }

void GestureRecorder::set_pitch(int val) {
  _mtx.lock();
  _frame.pitch = val;
  _mtx.unlock();
}

void GestureRecorder::set_pressure(int val) {
  _mtx.lock();
  _frame.pressure = val;
  _mtx.unlock();
}

void GestureRecorder::set_timbre(int val) {
  _mtx.lock();
  _frame.timbre = val;
  _mtx.unlock();
}

void GestureRecorder::set_pitch_mod(float val) { rand_modulation_pitch_ = val; }

void GestureRecorder::set_pressure_mod(float val) {
  rand_modulation_pressure_ = val;
}

void GestureRecorder::set_timbre_mod(float val) {
  rand_modulation_timbre_ = val;
}

void GestureRecorder::set_modulation(float val) { rand_modulation_ = val; }

void GestureRecorder::record(tick_t tick) {
  if (_is_recording) {
    _mtx.lock();
    _ptn->active_track().voice.record(tick, _frame);
    _mtx.unlock();
  }
}
}  // namespace GestureLooper
