/*

          ___  
         (   ) 
  .--.    | |  
 /    \   | |  
;  ,-. '  | |  
| |  | |  | |  
| |  | |  | |  
| |  | |  | |  
| '  | |  | |  
'  `-' |  | |  
 `.__. | (___) 
 ( `-' ;       
  `.__.        

Gesture Looper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

ParameterLayout.hpp
Parameter settings

*/

#pragma once

#include <array>

namespace GestureLooper {

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

}  // namespace GestureLooper
