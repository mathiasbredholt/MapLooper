/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

ParameterLayout.hpp
Parameter settings

*/

#pragma once

#include <array>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace MapLooper {
const int NUM_PATTERNS{1};
const int NUM_TRACKS{1};
const int NUM_TRACK_PARAMS{4};

enum param_name_t {
  //  TRACK PARAMETERS
  STEPS = 0,
  VELOCITY,
  DIVISION,
  SUSTAIN,
  PATTERN_LENGTH,  //  PATTERN PARAMETERS
  QUANTIZE,
  TEMPO  //  OTHERS
};

extern const std::array<int, 16> DIVISION_PRESETS;

extern const std::array<int, 16> REPEATS_TIME_PRESETS;

extern const std::array<int, 16> PTN_LENGTH_PRESETS;

typedef uint32_t Tick;

typedef std::function<void(int trackId, const std::string& path, float value)>
    SignalCallback;

typedef std::unordered_map<std::string, float> SignalDataMap;

}  // namespace MapLooper
