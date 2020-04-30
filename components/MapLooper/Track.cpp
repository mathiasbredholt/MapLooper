/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Track.cpp
Class to represent Euclidean tracks with groove system

*/

#include "MapLooper/Track.hpp"

namespace MapLooper {
std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS>
    Track::off_event_queue;

Track::Track() : voice(get_mpe_channel(), 768) {}

bool Track::set_param_relative(param_name_t name, int val) {
  return params.set_param_relative(name, val);
}

void Track::set_param(param_name_t name, int val, bool clear) {
  params.params[name].set(val);
}

void Track::update(tick_t tick, int32_t ptn_length) {
  if (tick == tick / get_div() * get_div()) {
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

  // // Handle note off events
  // while (off_event_queue[id].size() &&
  //        t >= off_event_queue[id].top().time_stamp) {
  //   midi::message_t msg;
  //   off_event_t e = off_event_queue[id].top();
  //   midi::note_off(&msg, e.pitch, e.channel);
  //   midi::send(&msg);
  //   off_event_queue[id].pop();
  // }
}

int Track::get_mpe_channel() { return clip<int>(id + 1, 1, 15); }

void Track::play_note(note_t* n, tick_t t) {
  midi::message_t msg;
  midi::note_on(&msg, n->pitch, n->velocity, get_mpe_channel());
  midi::send(&msg);

  // gesture_recorder.play(get_mpe_channel(), t->t, 768);
  // off_event_t off_event;
  // off_event.pitch = n->pitch;
  // off_event.time_stamp = n->note_off;
  // off_event.channel = get_mpe_channel();
  // off_event_queue[id].push(off_event);
}

void Track::release_notes() {
  midi::message_t msg;
  while (off_event_queue[id].size()) {
    off_event_t e = off_event_queue[id].top();
    midi::note_off(&msg, e.pitch, e.channel);
    midi::send(&msg);
    off_event_queue[id].pop();
  }
}

void Track::clear() {
  release_notes();
  for (int i = 0; i < NUM_TRACK_PARAMS; ++i) {
    clear_param(static_cast<param_name_t>(i));
  }
}

void Track::clear_param(param_name_t name) {
  params.clear_param(name);
}
}  // namespace MapLooper
