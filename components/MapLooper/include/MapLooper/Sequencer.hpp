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

#include <vector>

#include "MapLooper/Clock.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "esp_log.h"

namespace MapLooper {
class Sequencer {
 public:
  Sequencer(MidiOut* midiOut) : _midiOut(midiOut), pattern(_midiOut) {
    esp_log_level_set(_getTag(), ESP_LOG_WARN);
    _clock.setStartStopCallback([this](bool isPlaying) {
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
    _clock.start();
    if (!_clock.isLinked()) {
      _isPlaying = true;
      start_callback_();
    }
  }

  void stop() {
    _clock.stop();
    if (!_clock.isLinked()) {
      _isPlaying = false;
      stop_callback_();
    }
  }

  void update() {
    // active_sensing_();
    int32_t clk = _clock.getTicks();
    if (clk < 0) return;

    _tick = clk;

    if (tick_function_.is_on_tick(_tick)) {
      // midiBeatClock(_tick);
      if (_isRecording) {
        pattern.getActiveTrack().record(_tick, _values);
      }

      if (_isPlaying) {
        pattern.update(_tick, _signalInfoMap);
      }
    }
  }

  const Clock& getClock() { return _clock; }

  Tick getTicks() { return _tick; }

  bool isPlaying() { return _isPlaying; }

  void setTempoRelative(int val) {
    _clock.setTempo(std::min(std::max<int>(_clock.getTempo() + val, 20), 255));
  }

  void setTempo(int val) { _clock.setTempo(val); }

  int getTempo() { return _clock.getTempo(); }

  void setRecording(bool enable) { _isRecording = enable; }

  void setValue(const std::string& path, float value) {
    _values[path] = value;
    ESP_LOGI(_getTag(), "Recorded: '%s' : %f", path.c_str(), value);
  }

  void addSignal(const std::string& path, const SignalInfo& signalInfo) {
    _signalInfoMap.emplace(path, signalInfo);
    ESP_LOGI(_getTag(), "Added info for '%s'", path.c_str());
  }

  void setActiveTrack(int id) {
    if (id >= 0 && id < NUM_TRACKS) {
      pattern.setActiveTrack(id);
    }
  }

  void setPlayState(int id, bool enable) {
    if (id >= 0 && id < NUM_TRACKS) {
      pattern.setActiveTrack(id);
    }
  }

 private:
  static const char* _getTag() { return "Sequencer"; };

  bool _isRecording{false};

  SignalInfoMap _signalInfoMap;

  SignalDataMap _values;

  MidiOut* _midiOut;

  Pattern pattern;

  int32_t last_active_sensing_{0};

  Tick _tick;

  TickFunction tick_function_;

  Clock _clock;

  bool _isPlaying = false;

  void midiBeatClock(int32_t t) {
    if (mod(t, 4) == 0) {
      _midiOut->beat_clock();
    }
  }

  void start_callback_() {
    _clock.reset();
    _midiOut->start();
  }

  void stop_callback_() {
    pattern.releaseAll();
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
};

}  // namespace MapLooper
