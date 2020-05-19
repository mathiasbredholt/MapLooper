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
#include <vector>

namespace MapLooper {
const int NUM_PATTERNS{1};
const int NUM_TRACKS{1};
const int NUM_TRACK_PARAMS{4};

extern const std::array<int, 16> DIVISION_PRESETS;

typedef uint32_t Tick;

}  // namespace MapLooper
