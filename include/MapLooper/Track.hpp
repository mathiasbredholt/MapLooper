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

#include "MapLooper/Config.hpp"
#include "MapLooper/Modulation.hpp"
#include "MapLooper/Signal.hpp"
#include "MapLooper/Util.hpp"
#include "esp_log.h"
#include "esp_attr.h"

namespace MapLooper {

const int MAX_EVENTS = 64;
const int MAX_LENGTH = 768;

typedef std::unordered_map<std::string, float> Frame;
typedef std::array<Frame, MAX_LENGTH> FrameArray;

class Track {
 public:
  Track(int _id) { esp_log_level_set(_getTag(), ESP_LOG_WARN); }

  void record(Tick tick, const Frame& values) {
    _frameArray[tick % _length] = values;
  }

  void IRAM_ATTR update(Tick tick, const Signal::Map& map) {
    _modulation.update(tick, _length);

    tick %= _length;
    Frame frame = _frameArray.at(tick);

    for (const auto& f : frame) {
      ESP_LOGI(_getTag(), "'%s': %f", f.first.c_str(), f.second);
      const Signal& signal = map.at(f.first);
      signal.getCallback()(_id, f.first,
                           _modulation.get(f.second, tick, signal));
    }
  }

  void setLength(float beats) {
    _length = std::round(beats * Clock::TICKS_PER_QUARTER_NOTE);
  }

  void setEnabled(bool state) { _isEnabled = state; }

  bool getEnabled() { return _isEnabled; }

 private:
  inline static const char* _getTag() { return "Track"; };

  FrameArray _frameArray;

  Modulation _modulation;

  int _length{MAX_LENGTH};

  uint8_t _id{0};

  bool _isEnabled{true};
};

}  // namespace MapLooper
