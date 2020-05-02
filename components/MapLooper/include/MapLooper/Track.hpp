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

#include "MapLooper/EventQueue.hpp"
#include "MapLooper/GestureVoice.hpp"
#include "MapLooper/Parameter.hpp"
#include "MapLooper/ParameterLayout.hpp"
#include "MapLooper/TrackParameters.hpp"
#include "MapLooper/Util.hpp"
#include "esp_attr.h"

namespace MapLooper {

const int MAX_EVENTS = 64;

class Pattern;

class Track {
  friend Pattern;
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

  uint8_t id{0};

  TrackParameters params;

  GestureVoice voice;

  typedef std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS> OffEventQueueType;

  Track() : voice(get_mpe_channel(), 768) {}

  static int get_division_preset(int idx) {
    static int PRESETS[] = {384, 192, 96, 48, 24, 12, 6, 6, 128, 128, 128, 64, 32, 16, 8, 8};
    return PRESETS[idx];
  }

  int get_param(param_name_t name) const { return params.get_param(name); }

  int get_div() const { return get_division_preset(get_param(DIVISION)); }

  bool set_param_relative(param_name_t name, int val) {
    return params.set_param_relative(name, val);
  }

  void set_param(param_name_t name, int val, bool clear) {
    params.params[name].set(val);
  }

  void update(tick_t tick, int32_t ptn_length) {
    if (tick % get_div() == 0) {
      note_t n;
      n.pitch = 60;
      n.velocity = 127;
      n.note_off = tick + get_div() - 3;
      play_note(&n, tick);
    }

    voice.update(tick);
    // tick_t tick;
    // calc_tick(&tick, t);
    // render_tick(&tick);

    // Handle note off events
    while (getOffEventQueue()[id].size() &&
           tick >= getOffEventQueue()[id].top().time_stamp) {
      off_event_t e = getOffEventQueue()[id].top();
      _midiOut->note_off(e.pitch, e.channel);
      getOffEventQueue()[id].pop();
    }
  }

  int get_mpe_channel() { return clip<int>(id + 1, 1, 15); }

  void play_note(note_t* n, tick_t t) {
    _midiOut->note_on(n->pitch, n->velocity, get_mpe_channel());

    off_event_t off_event;
    off_event.pitch = n->pitch;
    off_event.time_stamp = n->note_off;
    off_event.channel = get_mpe_channel();
    getOffEventQueue()[id].push(off_event);
  }

  void release_notes() {
    while (getOffEventQueue()[id].size()) {
      off_event_t e = getOffEventQueue()[id].top();
      _midiOut->note_off(e.pitch, e.channel);
      getOffEventQueue()[id].pop();
    }
  }

  void clear() {
    release_notes();
    for (int i = 0; i < NUM_TRACK_PARAMS; ++i) {
      clear_param(static_cast<param_name_t>(i));
    }
  }

  void clear_param(param_name_t name) { params.clear_param(name); }

  static OffEventQueueType& getOffEventQueue() {
    static OffEventQueueType offEventQueue;
    return offEventQueue;
  }

private:
  MidiOut* _midiOut;
};

}  // namespace MapLooper
