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

GestureVoice.cpp
Description

*/

#include "GestureLooper/GestureVoice.hpp"

namespace GestureLooper {
GestureVoice::GestureVoice(int channel, int length) : channel_(channel), _length(length) {
  for (int i = 0; i < MAX_DATA_SIZE; ++i) {
    // _data[i].pitch = std::fmod(i / 384.0f, 384.0f);
    _data[i].pitch = 0;
    _data[i].pressure = 0;
    _data[i].timbre = 0;
    // _data[i].pressure = _data[i].pitch;
    // _data[i].timbre = _data[i].pitch;
  }
}

void GestureVoice::update(tick_t tick) {
  tick %= _length;
  midi::message_t msg;

  if (_data[tick].pitch != _last_frame.pitch) {
    // printf("tick: %d, pitch: %d\n", tick, _data[tick].pitch);
    midi::pitch_bend(&msg, _data[tick].pitch, channel_);
    midi::send(&msg);
  }

  if (_data[tick].pressure != _last_frame.pressure) {
    midi::pressure(&msg, _data[tick].pressure, channel_);
    midi::send(&msg);
  }

  if (_data[tick].timbre != _last_frame.timbre) {
    midi::timbre(&msg, _data[tick].timbre, channel_);
    midi::send(&msg);
  }

  _last_frame = _data[tick];
}

void GestureVoice::record(tick_t tick, gesture_frame_t frame) {
    _data[tick % _length] = frame;
    // printf("tick: %d, pitch: %d\n", tick, _data[tick % _length].pitch);
}

}  // namespace GestureLooper
