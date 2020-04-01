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

Track.cpp
Class to represent Euclidean tracks with groove system

*/

#include "GestureLooper/Track.hpp"

namespace GestureLooper {
std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS>
    Track::off_event_queue;

Track::Track() {}

bool Track::set_param_relative(param_name_t name, int val) {
  return params.set_param_relative(name, val);
}

void Track::set_param(param_name_t name, int val, bool clear) {
  params.params[name].set(val);
}

void Track::update(int32_t t, int32_t ptn_length) {
  tick_t tick;
  calc_tick(&tick, t);
  render_tick(&tick);
  gesture_recorder.update(t);

  // Handle note off events
  while (off_event_queue[id].size() &&
         t >= off_event_queue[id].top().time_stamp) {
    midi::message_t msg;
    off_event_t e = off_event_queue[id].top();
    midi::note_off(&msg, e.pitch, e.channel);
    midi::send(&msg, midi::ALL);
    off_event_queue[id].pop();
  }
}

void Track::calc_tick(tick_t* tick, int32_t t) {
  tick->t = t;
}

void Track::render_tick(tick_t* tick) {
  if (tick->t == tick->t / get_div() * get_div()) {
    note_t n;
    n.pitch = 60;
    n.velocity = 127;
    n.note_off = tick->t + get_div() - 3;
    play_note(&n, tick);
  }
}

int Track::get_mpe_channel() { return clip<int>(id + 1, 1, 15); }

void Track::play_note(note_t* n, tick_t* t) {
  midi::message_t msg;
  midi::note_on(&msg, n->pitch, n->velocity, get_mpe_channel());
  midi::send(&msg, midi::ALL);

  gesture_recorder.play(get_mpe_channel(), t->t, get_div());

  off_event_t off_event;
  off_event.pitch = n->pitch;
  off_event.time_stamp = n->note_off;
  off_event.channel = get_mpe_channel();
  off_event_queue[id].push(off_event);
}

void Track::release_notes() {
  midi::message_t msg;
  while (off_event_queue[id].size()) {
    off_event_t e = off_event_queue[id].top();
    midi::note_off(&msg, e.pitch, e.channel);
    midi::send(&msg, midi::ALL);
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
}  // namespace GestureLooper
