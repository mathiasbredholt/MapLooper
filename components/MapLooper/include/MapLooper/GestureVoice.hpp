/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

GestureVoice.hpp
Voice of MPE data

*/

#pragma once

#include <array>

#include "MapLooper/midi/MidiConfig.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
struct gesture_frame_t {
  int32_t pitch = 0;
  int32_t pressure = 0;
  int32_t timbre = 0;
};

static const int MAX_DATA_SIZE = 768;

typedef std::array<gesture_frame_t, MAX_DATA_SIZE> GestureData;

class GestureVoice {
 public:
  GestureVoice(int channel, int length) : _channel(channel), _length(length) {
    for (int i = 0; i < MAX_DATA_SIZE; ++i) {
      // _data[i].pitch = std::fmod(i / 384.0f, 384.0f);
      _data[i].pitch = 0;
      _data[i].pressure = 0;
      _data[i].timbre = 0;
      // _data[i].pressure = _data[i].pitch;
      // _data[i].timbre = _data[i].pitch;
    }
  }

  void update(tick_t tick) {
    tick %= _length;
    if (_data[tick].pitch != _last_frame.pitch) {
      _midiOut->pitch_bend(_data[tick].pitch, _channel);
    }

    if (_data[tick].pressure != _last_frame.pressure) {
      _midiOut->pressure(_data[tick].pressure, _channel);
    }

    if (_data[tick].timbre != _last_frame.timbre) {
      _midiOut->timbre(_data[tick].timbre, _channel);
    }

    _last_frame = _data[tick];
  }

  void record(tick_t tick, gesture_frame_t frame) {
      _data[tick % _length] = frame;
  }

 private:
  GestureData _data;

  gesture_frame_t _next_frame;

  gesture_frame_t _last_frame;

  int _channel{0};

  int _length;

  MidiOut* _midiOut;
};
}  // namespace MapLooper
