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

#include <functional>
#include <unordered_map>

namespace MapLooper {

typedef std::function<void(int trackId, const std::string& path, float value)>
    SignalCallback;

class Signal {
 public:
  Signal(SignalCallback callback, float minValue, float maxValue)
      : _callback(callback), _minValue(minValue), _maxValue(maxValue) {}

  inline const SignalCallback& getCallback() const { return _callback; }

  inline float getRange() const { return _maxValue - _minValue; }

  inline float getMin() const { return _minValue; }

  inline float getMax() const { return _maxValue; }

 private:
  SignalCallback _callback;
  float _minValue, _maxValue;
};  // namespace MapLooper

typedef std::unordered_map<std::string, Signal> SignalMap;
}  // namespace MapLooper
