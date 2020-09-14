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

#include "MapLooper/Clock.hpp"
#include "MapLooper/Scene.hpp"
#include "MapLooper/Signal.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace MapLooper {
class Sequencer {
 public:
  Sequencer() {
    xTaskCreate(
        [](void* userParam) {
          Sequencer* sequencer = static_cast<Sequencer*>(userParam);
          for (;;) {
            sequencer->update();
            vTaskDelay(1);
          }
        },
        "Sequencer", 4096, this, 19, &_task);

    _clock.setStartStopCallback([this](bool isPlaying) {
      _isPlaying = isPlaying;
      if (isPlaying) {
        _startCallback();
      }
    });
    start();
  }

  ~Sequencer() { vTaskDelete(_task); }

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

    if (_tickFunction.is_on_tick(tick)) {
      // printf("%d\n", tick);
      if (_isRecording) {
        _scene.getActiveTrack()->record(tick, _values);
      }
      if (_isPlaying) {
        _scene.update(tick, _map);
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

  void setValue(const std::string& path, float value) { _values[path] = value; }

  void addSignal(const std::string& path, float min, float max,
                 Signal::Callback signalCallback, mpr_dev dev) {
    Signal signal(
        path, min, max, signalCallback,
        [](Signal* signal) {
          // ESP_LOGI(_getTag(), "setValueCallback: %p", signal);
          signal->getSequencer()->setValue(signal->getPath(),
                                           signal->getValue());
        },
        dev, this);
    _map.emplace(path, signal);
    _map.at(path).updatePtr();
    ESP_LOGI(_getTag(), "Added signal '%s'", path.c_str());
  }

  void setActiveTrack(int id) { _scene.setActiveTrack(id); }

  void setLength(float value) { _scene.getActiveTrack()->setLength(value); }

  void setPlayState(bool state) { _scene.getActiveTrack()->setEnabled(state); }

 private:
  static const char* _getTag() { return "Sequencer"; };

  TaskHandle_t _task;

  Clock _clock;

  Scene _scene;

  Signal::Map _map;

  bool _isRecording{false};

  bool _isPlaying = false;

  Frame _values;

  TickFunction _tickFunction;

  void _startCallback() { _clock.reset(); }
};

}  // namespace MapLooper
