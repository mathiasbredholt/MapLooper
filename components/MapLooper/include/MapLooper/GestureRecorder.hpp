/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

GestureRecorder.hpp
Class to record gestures

*/

#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include "MapLooper/GestureVoice.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
class GestureRecorder {
 public:
  GestureRecorder(Pattern* pattern) : _pattern(pattern) {}

  void set_is_recording(bool enable) {
    _is_recording = enable;
  }

  // void play(int channel, int32_t t, int32_t duration) {
  //   // GestureVoice voice(channel, duration, t);
  //   // voices_.push_back(voice);
  // }

  void set_pitch(int val) {
    _frame.pitch = val;
  }

  void set_pressure(int val) {
    _frame.pressure = val;
  }

  void set_timbre(int val) {
    _frame.timbre = val;
  }

  void set_pitch_mod(float val) {
    rand_modulation_pitch_ = val;
  }

  void set_pressure_mod(float val) {
    rand_modulation_pressure_ = val;
  }

  void set_timbre_mod(float val) {
    rand_modulation_timbre_ = val;
  }

  void set_modulation(float val) { rand_modulation_ = val; }

  void record(tick_t tick) {
    if (_is_recording) {
      // _pattern->active_track().voice.record(tick, _frame);
    }
  }

 private:
  Pattern* _pattern;

  bool _is_recording{false};

  gesture_frame_t _frame;

  float rand_modulation_pitch_{0};

  float rand_modulation_pressure_{0};

  float rand_modulation_timbre_{0};

  float rand_modulation_{0};
};
}  // namespace MapLooper
