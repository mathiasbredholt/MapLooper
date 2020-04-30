/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

MapLooper.cpp
Top level module

*/

#include "MapLooper/MapLooper.hpp"

namespace MapLooper {

MapLooper::MapLooper(mpr_dev* libmapper_device)
    : sequencer(&_ptn),
      mapper(libmapper_device, sequencer.get_recorder(), &sequencer) {
  midi::init();
}

void MapLooper::update() {
  // mapper.update();
  sequencer.update();
  midi::flush();
}

}  // namespace MapLooper
