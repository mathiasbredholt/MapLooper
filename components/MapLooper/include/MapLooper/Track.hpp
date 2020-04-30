/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Track.hpp
Class to represent Euclidean tracks with groove system

*/

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <climits>

#include "esp_attr.h"

#include "MapLooper/EventQueue.hpp"
#include "MapLooper/Parameter.hpp"
#include "MapLooper/ParameterLayout.hpp"
#include "MapLooper/TrackParameters.hpp"
#include "MapLooper/MIDI.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/GestureVoice.hpp"

namespace MapLooper {

const int MAX_EVENTS = 64;

class Pattern;

class Track {
 public:
 struct note_t {
    int32_t time_stamp;
    int i;
    int j;
    int8_t pitch;
    uint8_t velocity;
    int32_t note_off;
    uint8_t channel;
    bool is_top_note;
  };

  static std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS>
      off_event_queue;

  uint8_t id{0};

  TrackParameters params;

  GestureVoice voice;

  Track();

  int get_mpe_channel();

  bool set_param_relative(param_name_t name, int val);

  void set_param(param_name_t name, int val, bool clear);

  void clear();

  void clear_param(param_name_t name);

  void update(tick_t tick, int32_t ptn_length);

  void release_notes();

  void play_note(note_t* n, tick_t t);

  int get_param(param_name_t name) const;

  int get_div() const;
};

inline int Track::get_param(param_name_t name) const {
  return params.get_param(name);
}

inline int Track::get_div() const {
  return DIVISION_PRESETS[get_param(DIVISION)];
}

}  // namespace MapLooper
