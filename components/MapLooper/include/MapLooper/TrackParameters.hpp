/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

TrackParameters.hpp
Class to represent the parameters on a Track

*/

#pragma once

#include "MapLooper/Parameter.hpp"
#include "MapLooper/ParameterLayout.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
class TrackParameters {
 public:
  static const int MAX_STEP_SIZE = 32;

  bool set_param_relative(param_name_t name, int val);

  void set_param(param_name_t name, int val);

  int get_param(param_name_t name) const;

  void clear_param(param_name_t name);

  std::array<Parameter<uint8_t>, NUM_TRACK_PARAMS> params;
};

inline int TrackParameters::get_param(param_name_t name) const {
  if (name >= NUM_TRACK_PARAMS) {
    return 0;
  }
  return params[name].get();
}
}  // namespace MapLooper
