/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Modulation.hpp
Pseudo-random modulation module

*/

#pragma once

#include "MapLooper/SignalInfo.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
class Modulation {
 public:
  Modulation() {}

  void update(Tick tick, int patternLength) {
    if (tick % _div == 0) {
      const int idx = (tick / _div) % RESOLUTION;
      _updatePoint(idx);
      const int modulationLength = _div * RESOLUTION;
      if (patternLength < modulationLength) {
        int n = RESOLUTION - (modulationLength - patternLength) / _div;
        for (int i = n; i < RESOLUTION; ++i) {
          _updatePoint(i);
        }
      }
    }
  }

  inline float getModulation(float value, Tick tick,
                             const SignalInfo& signalInfo) const {
    return clip<float>(
        value + _getValue(tick) * signalInfo.getRange() * _amount,
        signalInfo.getMin(), signalInfo.getMax());
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
