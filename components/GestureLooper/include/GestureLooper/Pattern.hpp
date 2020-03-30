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

Pattern.hpp
Class to contain a set of Tracks

*/

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>

#include "GestureLooper/Clock.hpp"
#include "GestureLooper/Parameter.hpp"
#include "GestureLooper/Track.hpp"

class Bank;
class PatternController;
class Torso;

namespace GestureLooper {
class Pattern {
  friend class Bank;
  friend class PatternController;
  friend class Torso;

 public:
  Pattern();

  uint8_t id{0};

  uint8_t active_track_id{0};

  Track* active_track();

  void clear();

  void update(int32_t t);

  void reset_to(int32_t time_point);

  void copy(Pattern* src);

  void release_all();

  int32_t get_local_ticks(int32_t ticks) const;

  int get_seq_idx(int32_t ticks);

  int get_param(param_name_t name);

  bool is_enabled(const Track& track);

  bool is_enabled(int id);

  bool is_finite();

  int get_length();

  int get_quantization();

  static Track* track_to_copy;

  int32_t reset_point{0};

  std::array<Track, NUM_TRACKS> tracks;

  std::bitset<NUM_TRACKS> mute_states{0xffff};

  std::bitset<NUM_TRACKS> next_mute_states{0xffff};

  Parameter<uint8_t> quantization;

  Parameter<uint8_t> length;
};

inline Track* Pattern::active_track() { return &tracks[active_track_id]; }

inline int32_t Pattern::get_local_ticks(int32_t ticks) const {
  return ticks - reset_point;
}

inline bool Pattern::is_enabled(const Track& track) {
  return !mute_states[track.id];
}

inline bool Pattern::is_enabled(int id) { return !mute_states[id]; }

inline int Pattern::get_seq_idx(int32_t ticks) {
  return mod((get_local_ticks(ticks)) / active_track()->get_div(),
             get_param(STEPS));
}

inline int Pattern::get_param(param_name_t name) {
  if (name == PATTERN_LENGTH) {
    return length.get();
  } else if (name == QUANTIZE) {
    return quantization.get();
  } else if (name < NUM_TRACK_PARAMS) {
    return active_track()->get_param(name);
  } else {
    return 0;
  }
}

inline bool Pattern::is_finite() { return length.get() != 8; }

inline int Pattern::get_length() { return PTN_LENGTH_PRESETS[length.get()]; }

inline int Pattern::get_quantization() {
  return PTN_LENGTH_PRESETS[quantization.get()];
}

}  // namespace GestureLooper
