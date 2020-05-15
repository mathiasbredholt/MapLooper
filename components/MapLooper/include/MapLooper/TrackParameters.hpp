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
#include "MapLooper/Config.hpp"
#include "MapLooper/Util.hpp"

namespace MapLooper {
class TrackParameters {
 public:
  static const int MAX_STEP_SIZE = 32;

  int getParam(param_name_t name) const {
    if (name >= NUM_TRACK_PARAMS) {
      return 0;
    }
    return params[name].get();
  }

  bool setParamRelative(param_name_t name, int val) {
    return false;
  }

  void setParam(param_name_t name, int val) {
  }

  void clearParam(param_name_t name) {
    switch (name) {
      case STEPS:
        params[name].set(16);
        break;
      case VELOCITY:
        params[name].set(100);
        break;
      case SUSTAIN:
        params[name].set(127);
        break;
      case DIVISION:
        params[name].set(0);
        break;
      default:
        break;
    }
  }

  std::array<Parameter<uint8_t>, NUM_TRACK_PARAMS> params;
};
}  // namespace MapLooper
