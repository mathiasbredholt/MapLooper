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

Sequencer.cpp
Class to sequence patterns

*/

#include "GestureLooper/Sequencer.hpp"

namespace GestureLooper {

Sequencer::Sequencer(Pattern* ptn)
    : on_next_ptn_in_chain([]() {}), ptn_(ptn) {
  clock_.set_start_stop_callback([this](bool is_playing) {
    is_playing_ = is_playing;
    if (is_playing) {
      start_callback_();
    } else {
      stop_callback_();
    }
  });
  start();
}

void Sequencer::start() {
  clock_.start();
  if (!clock_.is_linked()) {
    is_playing_ = true;
    start_callback_();
  }
}

void Sequencer::stop() {
  clock_.stop();
  if (!clock_.is_linked()) {
    is_playing_ = false;
    stop_callback_();
  }
}

void Sequencer::start_callback_() {
  clock_.reset();

  midi::message_t msg;
  midi::start(&msg);
  midi::send(&msg, midi::ALL);;
}

void Sequencer::stop_callback_() {
  ptn_->release_all();

  midi::message_t msg;
  midi::stop(&msg);
  midi::send(&msg, midi::ALL);

  // for (int i = 0; i < 16; ++i) {
  //   midi::all_sound_off(&msg, i);
  //   midi::send(&msg, midi::ALL);
  // }
}

void Sequencer::active_sensing_() {
  uint32_t now = millis();
  if (now - last_active_sensing_ > 300) {
    midi::message_t msg;
    midi::active_sensing(&msg);
    midi::send(&msg, midi::ALL);
    last_active_sensing_ = now;
  }
}

void Sequencer::update() {
  // active_sensing_();

  ticks_ = clock_.get_ticks();

  if (tick_function_.is_on_tick(ticks_)) {
    // midi_beat_clock_(ticks_);

    if (ticks_ % 96 == 0) {
      printf("ticks: %d, tempo: %d\n", ticks_, get_tempo());
    }

    if (is_playing_ && ticks_ >= 0) {
      ptn_->update(ticks_);
    }

    midi::flush();
  }
}

void Sequencer::midi_beat_clock_(int32_t t) {
  if (mod(t, 4) == 0) {
    midi::message_t msg;
    midi::beat_clock(&msg);
    midi::send(&msg, midi::ALL);
  }
}

const Clock& Sequencer::get_clock() { return clock_; }

int32_t Sequencer::get_ticks() { return ticks_; }

bool Sequencer::is_playing() { return is_playing_; }

void Sequencer::set_tempo_relative(int val) {
  clock_.set_tempo(std::min(std::max<int>(clock_.get_tempo() + val, 20), 255));
}

void Sequencer::set_tempo(int val) { clock_.set_tempo(val); }

int Sequencer::get_tempo() { return clock_.get_tempo(); }

}  // namespace GestureLooper
