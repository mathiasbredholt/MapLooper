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

#include "MapLooper/Pattern.hpp"
#include "MapLooper/Sequencer.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "MapLooper/mpr/Mapper.hpp"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper(mpr_dev* mpr_device)
      : sequencer(&midiOut), mapper(mpr_device, &sequencer) {}

  void update() {
    mapper.update();
    sequencer.update();
    midiOut.flush();
  }

  MidiOut midiOut;
  Sequencer sequencer;
  Mapper mapper;
};
}  // namespace MapLooper
