/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Sequencer.hpp
Class to sequence patterns

*/

#pragma once

#include <functional>
#include <vector>

#include "MapLooper/Clock.hpp"
#include "MapLooper/GestureRecorder.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/midi/MidiConfig.hpp"

namespace MapLooper {
class Sequencer {
 public:
  Sequencer(Pattern* pattern, MidiOut* midiOut)
      : _ptn(pattern), _midiOut(midiOut) {
    clock_.set_start_stop_callback([this](bool isPlaying) {
      _isPlaying = isPlaying;
      if (isPlaying) {
        start_callback_();
      } else {
        stop_callback_();
      }
    });
    start();
  }

  void start() {
    clock_.start();
    if (!clock_.is_linked()) {
      _isPlaying = true;
      start_callback_();
    }
  }

  void stop() {
    clock_.stop();
    if (!clock_.is_linked()) {
      _isPlaying = false;
      stop_callback_();
    }
  }

  void start_callback_() {
    clock_.reset();
    _midiOut->start();
  }

  void stop_callback_() {
    _ptn->release_all();
    _midiOut->stop();

    // for (int i = 0; i < 16; ++i) {
    //   midi::all_sound_off(&msg, i);
    //   midi::send(&msg);
    // }
  }

  void active_sensing_() {
    uint32_t now = millis();
    if (now - last_active_sensing_ > 300) {
      _midiOut->active_sensing();
      last_active_sensing_ = now;
    }
  }

  void update() {
    // active_sensing_();
    int32_t clk = clock_.get_ticks();
    if (clk < 0) return;

    _tick = clk;

    if (tick_function_.is_on_tick(_tick)) {
      // midi_beat_clock_(_tick);
      if (_isPlaying) {
        _recorder->record(_tick);
        _ptn->update(_tick);
      }
    }
  }

  void midi_beat_clock_(int32_t t) {
    if (mod(t, 4) == 0) {
      _midiOut->beat_clock();
    }
  }

  const Clock& get_clock() { return clock_; }

  tick_t get_ticks() { return _tick; }

  bool isPlaying() { return _isPlaying; }

  void set_tempo_relative(int val) {
    clock_.set_tempo(
        std::min(std::max<int>(clock_.get_tempo() + val, 20), 255));
  }

  void set_tempo(int val) { clock_.set_tempo(val); }

  int get_tempo() { return clock_.get_tempo(); }

 private:
  Pattern* _ptn;

  MidiOut* _midiOut;

  int32_t last_active_sensing_{0};

  tick_t _tick;


  GestureRecorder* _recorder;

  TickFunction tick_function_;

  Clock clock_;

  bool _isPlaying = false;
};

}  // namespace MapLooper
