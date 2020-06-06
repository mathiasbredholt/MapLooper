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
#include "MapLooper/TickTimer.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace MapLooper {
class Sequencer {
 public:
  Sequencer()
      : tickTimer(
            [](void* userParam) {
              Sequencer* sequencer = static_cast<Sequencer*>(userParam);
              for (;;) {
                ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                sequencer->update();
              }
            },
            this) {
    esp_log_level_set(_getTag(), ESP_LOG_WARN);
    _clock.setStartStopCallback([this](bool isPlaying) {
      _isPlaying = isPlaying;
      if (isPlaying) {
        _startCallback();
      }
    });
    start();
  }

  void start() {
    _clock.start();
    if (!_clock.isLinked()) {
      _isPlaying = true;
      _startCallback();
    }
  }

  void stop() {
    _clock.stop();
    if (!_clock.isLinked()) {
      _isPlaying = false;
    }
  }

  void update() {
    int32_t clk = _clock.getTicks();
    if (clk < 0) return;
    Tick tick = clk;

    if (tick_function_.is_on_tick(tick)) {
      if (_isRecording) {
        _scene.getActiveTrack().record(tick, _values);
      }

      if (_isPlaying) {
        _scene.update(tick, _signalMap);
      }
    }
  }

  const Clock& getClock() { return _clock; }

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
  static const char* _getTag() { return "Sequencer"; };

  Clock _clock;

  Scene _scene;

  TickTimer tickTimer;

  SignalMap _signalMap;

  bool _isRecording{false};

  bool _isPlaying = false;

  Frame _values;

  TickFunction tick_function_;

  void _startCallback() { _clock.reset(); }
};

}  // namespace MapLooper
