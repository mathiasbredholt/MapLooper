/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Pattern.hpp
Class to contain a set of Tracks

*/

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>

#include "esp_heap_caps.h"

#include "MapLooper/Clock.hpp"
#include "MapLooper/Parameter.hpp"
#include "MapLooper/Track.hpp"

class Bank;
class PatternController;
class Torso;

namespace MapLooper {
class Pattern {
  friend class Bank;
  friend class PatternController;
  friend class Torso;

 public:
  uint8_t id{0};
  
  uint8_t active_track_id{0};

  Pattern(MidiOut* midiOut) : _midiOut(midiOut) {
    for (int i = 0; i < NUM_TRACKS; ++i) {
      tracks[i].id = i;
      tracks[i]._midiOut = _midiOut;
    }
    mute_states[0] = false;
    next_mute_states[0] = false;
    clear();
  }

  void update(tick_t tick) {
    // if (is_finite() && t % get_length() == 0) {
    //   reset_to(t / get_length() * get_length());
    // }
    // Schedule tracks
    for (Track& t : tracks) {
      t.update(tick - reset_point, get_length());
    }
  }

  void reset_to(int32_t time_point) {
    reset_point = time_point;
    for (Track& r : tracks) {
      r.release_notes();
    }
  }

  void clear() {
    mute_states.set();
    next_mute_states.set();
    length.set(8);
    quantization.set(8);
    for (Track& r : tracks) {
      r.clear();
    }
    mute_states[0] = false;
    next_mute_states[0] = false;
    active_track_id = 0;
  }

  void release_all() {
    for (Track& r : tracks) {
      r.release_notes();
    }
  }

  Track& active_track() { return tracks[active_track_id]; }

  int32_t get_local_ticks(int32_t ticks) const {
    return ticks - reset_point;
  }

  bool is_enabled(const Track& track) {
    return !mute_states[track.id];
  }

  bool is_enabled(int id) { return !mute_states[id]; }

  int get_seq_idx(int32_t ticks) {
    return mod((get_local_ticks(ticks)) / active_track().get_div(),
               get_param(STEPS));
  }

  int get_param(param_name_t name) {
    if (name == PATTERN_LENGTH) {
      return length.get();
    } else if (name == QUANTIZE) {
      return quantization.get();
    } else if (name < NUM_TRACK_PARAMS) {
      return active_track().get_param(name);
    } else {
      return 0;
    }
  }

  bool is_finite() { return length.get() != 8; }

  int get_length() { return PTN_LENGTH_PRESETS[length.get()]; }

  int get_quantization() {
    return PTN_LENGTH_PRESETS[quantization.get()];
  }

  static Track* track_to_copy;

  tick_t reset_point{0};

  std::array<Track, NUM_TRACKS> tracks;

  std::bitset<NUM_TRACKS> mute_states{0xffff};

  std::bitset<NUM_TRACKS> next_mute_states{0xffff};

  Parameter<uint8_t> quantization;

  Parameter<uint8_t> length;

  MidiOut* _midiOut;
};
}  // namespace MapLooper
