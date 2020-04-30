/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Pattern.cpp
Class to contain a set of Tracks

*/

#include "MapLooper/Pattern.hpp"

namespace MapLooper {
Pattern::Pattern() {
  for (int i = 0; i < NUM_TRACKS; ++i) {
    tracks[i].id = i;
  }
  mute_states[0] = false;
  next_mute_states[0] = false;
  clear();
}

void Pattern::update(tick_t tick) {
  // if (is_finite() && t % get_length() == 0) {
  //   reset_to(t / get_length() * get_length());
  // }
  // Schedule tracks
  for (Track& t : tracks) {
    t.update(tick - reset_point, get_length());
  }
}

void Pattern::reset_to(int32_t time_point) {
  reset_point = time_point;
  for (Track& r : tracks) {
    r.release_notes();
  }
}

void Pattern::clear() {
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

void Pattern::release_all() {
  for (Track& r : tracks) {
    r.release_notes();
  }
}

Track& Pattern::active_track() { return tracks[active_track_id]; }

int32_t Pattern::get_local_ticks(int32_t ticks) const {
  return ticks - reset_point;
}

bool Pattern::is_enabled(const Track& track) {
  return !mute_states[track.id];
}

bool Pattern::is_enabled(int id) { return !mute_states[id]; }

int Pattern::get_seq_idx(int32_t ticks) {
  return mod((get_local_ticks(ticks)) / active_track().get_div(),
             get_param(STEPS));
}

int Pattern::get_param(param_name_t name) {
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

bool Pattern::is_finite() { return length.get() != 8; }

int Pattern::get_length() { return PTN_LENGTH_PRESETS[length.get()]; }

int Pattern::get_quantization() {
  return PTN_LENGTH_PRESETS[quantization.get()];
}

}  // namespace MapLooper
