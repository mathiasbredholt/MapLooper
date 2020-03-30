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

Pattern.cpp
Class to contain a set of Tracks

*/

#include "GestureLooper/Pattern.hpp"

namespace GestureLooper {
Pattern::Pattern() {
  for (int i = 0; i < NUM_TRACKS; ++i) {
    tracks[i].id = i;
  }
  mute_states[0] = false;
  next_mute_states[0] = false;
}

void Pattern::update(int32_t t) {
  if (is_finite() && t % get_length() == 0) {
    reset_to(t / get_length() * get_length());
  }
  // Schedule tracks
  for (Track& r : tracks) {
    if (r.id == active_track_id) {
      r.gesture_recorder.record_gestures(t, r.get_param(STEPS) * r.get_div());
    }
    
    if (is_enabled(r)) {
      r.update(t - reset_point, get_length());
    }
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
}  // namespace GestureLooper
