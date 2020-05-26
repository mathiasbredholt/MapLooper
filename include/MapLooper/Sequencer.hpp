/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

#include <vector>

#include "MapLooper/Clock.hpp"
#include "MapLooper/Scene.hpp"
#include "MapLooper/Signal.hpp"
#include "MapLooper/Track.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "esp_log.h"
#include "freertos/task.h"

namespace MapLooper {
class Sequencer {
 public:
  Sequencer(MidiOut* midiOut)
      : _midiOut(midiOut),
        _scene(_midiOut),
        tickTimer(
            [](void* userParam) {
              Sequencer* sequencer = static_cast<Sequencer*>(userParam);
              for (;;) {
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                sequencer->update();
                sequencer->_midiOut->flush();
              }
            },
            this) {
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
        _scene.getActiveTrack().record(_tick, _values);
      }

      if (_isPlaying) {
        _scene.update(_tick, _signalMap);
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

  void addSignal(const std::string& path, const Signal& signal) {
    _signalMap.emplace(path, signal);
    ESP_LOGI(_getTag(), "Added info for '%s'", path.c_str());
  }

  void setActiveTrack(int id) {
    if (id >= 0 && id < NUM_TRACKS) {
      _scene.setActiveTrack(id);
    }
  }

  void setPlayState(bool state) { _scene.getActiveTrack().setEnabled(state); }

 private:
  MidiOut _midiOut;

  Scene _scene;

  TickTimer tickTimer;

  static const char* _getTag() { return "Sequencer"; };

  bool _isRecording{false};

  SignalMap _signalMap;

  SignalDataMap _values;

  int32_t last_active_sensing_{0};

  Tick _tick;

  TickFunction tick_function_;

  Clock _clock;

  bool _isPlaying = false;

  void midiBeatClock(int32_t t) {
    if (mod(t, 4) == 0) {
      _midiOut.beat_clock();
    }
  }

  void start_callback_() {
    _clock.reset();
    _midiOut.start();
  }

  void stop_callback_() {
    _scene.releaseAll();
    _midiOut.stop();

    // for (int i = 0; i < 16; ++i) {
    //   midi::all_sound_off(&msg, i);
    //   midi::send(&msg);
    // }
  }

  void active_sensing_() {
    uint32_t now = millis();
    if (now - last_active_sensing_ > 300) {
      _midiOut.active_sensing();
      last_active_sensing_ = now;
    }
  }
};

}  // namespace MapLooper
