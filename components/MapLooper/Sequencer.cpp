/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Sequencer.cpp
Class to sequence patterns

*/

#include "MapLooper/Sequencer.hpp"

namespace MapLooper {

Sequencer::Sequencer(Pattern* ptn) : _ptn(ptn), _recorder(ptn) {
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
  midi::send(&msg);
  ;
}

void Sequencer::stop_callback_() {
  _ptn->release_all();

  midi::message_t msg;
  midi::stop(&msg);
  midi::send(&msg);

  // for (int i = 0; i < 16; ++i) {
  //   midi::all_sound_off(&msg, i);
  //   midi::send(&msg);
  // }
}

void Sequencer::active_sensing_() {
  uint32_t now = millis();
  if (now - last_active_sensing_ > 300) {
    midi::message_t msg;
    midi::active_sensing(&msg);
    midi::send(&msg);
    last_active_sensing_ = now;
  }
}

void Sequencer::update() {
  // active_sensing_();
  int32_t clk = clock_.get_ticks();
  if (clk < 0) return;
  
  _tick = clk;

  if (tick_function_.is_on_tick(_tick)) {
    // midi_beat_clock_(_tick);
    if (is_playing_) {
      _recorder.record(_tick);
      _ptn->update(_tick);
    }
  }
}

void Sequencer::midi_beat_clock_(int32_t t) {
  if (mod(t, 4) == 0) {
    midi::message_t msg;
    midi::beat_clock(&msg);
    midi::send(&msg);
  }
}

const Clock& Sequencer::get_clock() { return clock_; }

tick_t Sequencer::get_ticks() { return _tick; }

bool Sequencer::is_playing() { return is_playing_; }

void Sequencer::set_tempo_relative(int val) {
  clock_.set_tempo(std::min(std::max<int>(clock_.get_tempo() + val, 20), 255));
}

void Sequencer::set_tempo(int val) { clock_.set_tempo(val); }

int Sequencer::get_tempo() { return clock_.get_tempo(); }

GestureRecorder* Sequencer::get_recorder() { return &_recorder; }

}  // namespace MapLooper
