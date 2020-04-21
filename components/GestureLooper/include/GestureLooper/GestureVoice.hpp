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

GestureVoice.hpp
Voice of MPE data

*/

#pragma once

#include <array>

#include "GestureLooper/MIDI.hpp"
#include "GestureLooper/Util.hpp"

namespace GestureLooper {
struct gesture_frame_t {
  int32_t pitch = 0;
  int32_t pressure = 0;
  int32_t timbre = 0;
};

static const int MAX_DATA_SIZE = 768;

typedef std::array<gesture_frame_t, MAX_DATA_SIZE> GestureData;

class GestureVoice {
 public:
  GestureVoice(int channel, int length);

  void update(tick_t tick);

  void record(tick_t tick, gesture_frame_t frame);

 private:
  GestureData _data;

  gesture_frame_t _next_frame;

  gesture_frame_t _last_frame;

  bool _has_next_frame{false};

  int channel_{0};

  int _length;
};
}  // namespace GestureLooper
