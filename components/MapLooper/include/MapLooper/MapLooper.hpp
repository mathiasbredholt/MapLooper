/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

MapLooper.hpp
Top level module

*/

#pragma once

#include <iostream>

#include "MapLooper/Util.hpp"
#include "MapLooper/Mapper.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/Sequencer.hpp"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper(mpr_dev* libmapper_device);

  void update();

  Pattern _ptn;

  Sequencer sequencer;

  Mapper mapper;

};    
}  // namespace MapLooper
