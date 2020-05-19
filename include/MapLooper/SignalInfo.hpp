/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

SignalInfo.hpp
Class to contain information about a signal

*/

#pragma once

#include <functional>
#include <unordered_map>

namespace MapLooper {

typedef std::function<void(int trackId, const std::string& path, float value)>
    SignalCallback;

class SignalInfo {
 public:
  SignalInfo(SignalCallback callback, float minValue, float maxValue)
      : _callback(callback), _minValue(minValue), _maxValue(maxValue) {}

  inline const SignalCallback& getCallback() const { return _callback; }

  inline float getRange() const { return _maxValue - _minValue; }

  inline float getMin() const { return _minValue; }

  inline float getMax() const { return _maxValue; }

 private:
  SignalCallback _callback;
  float _minValue, _maxValue;
};  // namespace MapLooper

typedef std::unordered_map<std::string, SignalInfo> SignalInfoMap;
}  // namespace MapLooper
