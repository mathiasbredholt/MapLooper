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

#include "MapLooper/Signal.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
class Modulation {
 public:
  Modulation() {}

  void update(Tick tick, Tick trackLength) {
    if (tick % _div == 0) {
      const int idx = (tick / _div) % RESOLUTION;
      _updatePoint(idx);
      const int modulationLength = _div * RESOLUTION;
      if (trackLength < modulationLength) {
        int n = RESOLUTION - (modulationLength - trackLength) / _div;
        for (int i = n; i < RESOLUTION; ++i) {
          _updatePoint(i);
        }
      }
    }
  }

  inline float get(float value, Tick tick, const Signal& signal) const {
    return clip<float>(value + _getValue(tick) * signal.getRange() * _amount,
                       signal.getMin(), signal.getMax());
  }

  void setVariation(float value) { _variation = value; }

 private:
  inline float _getValue(Tick tick) const {
    return _data[(tick / _div) % RESOLUTION];
  }

  inline void _updatePoint(int idx) {
    float value = (esp_random() - 65536.0) / 4294967296.0;
    _data[idx] = fold(_data[idx] + (value * _variation), -1.0, 1.0);
  }

  static const int RESOLUTION = 64;
  Tick _div = 24;
  float _amount = 0.0f;
  float _variation = 0.0f;
  std::array<float, RESOLUTION> _data;
};
}  // namespace MapLooper
