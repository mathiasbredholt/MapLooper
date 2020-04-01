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
#include <random>
#include <vector>

#include "GestureLooper/MIDI.hpp"
#include "GestureLooper/Util.hpp"

namespace GestureLooper {
class GestureRecorder;
struct gesture_frame_t {
  float pitch = 0;
  float pressure = 0;
  float timbre = 0;
};

static const int MAX_DATA_SIZE = 384;

typedef std::array<gesture_frame_t, MAX_DATA_SIZE> GestureData;

class GestureVoice {
 public:
  explicit GestureVoice(GestureData* data, int channel,
                        int duration, int32_t start_time);

  void update(int32_t t);

  bool is_done(int32_t t);

 private:
  GestureData* _data;

  int channel_{0};

  int duration_{0};

  gesture_frame_t _last_data;

  int32_t start_time_{0};
};

class GestureRecorder {
  friend GestureVoice;

 public:
  GestureRecorder();

  void update(int32_t t);

  void record_gestures(int32_t t, int32_t duration);

  void play(int channel, int32_t t, int32_t duration);

  void set_is_recording(bool enable);

  void set_pitch(float val);

  void set_pressure(float val);

  void set_timbre(float val);

  void set_pitch_mod(float val);

  void set_pressure_mod(float val);

  void set_timbre_mod(float val);

  void set_modulation(float val);

  void reset();

 private:
  GestureData _data;

  // std::array<std::array<float, MAX_DATA_SIZE>, 3>
  //     rand_data_ = {{{{0}}, {{0}}, {{0}}}};

  bool is_recording_{false};

  float pitch_val_{0};

  float pressure_val_{0};

  float timbre_val_{0};

  float rand_modulation_pitch_{0};

  float rand_modulation_pressure_{0};

  float rand_modulation_timbre_{0};

  float rand_modulation_{0};

  std::vector<GestureVoice> voices_;

  static std::random_device rd_;

  std::mt19937 gen_;

  std::uniform_real_distribution<float> dist_;
};
}  // namespace GestureLooper
