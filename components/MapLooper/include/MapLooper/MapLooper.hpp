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

#include "MapLooper/GestureRecorder.hpp"
#include "MapLooper/Mapper.hpp"
#include "MapLooper/Pattern.hpp"
#include "MapLooper/Sequencer.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper(mpr_dev* mpr_device)
      : pattern(&midiOut),
        sequencer(&pattern, &midiOut),
        recorder(&pattern),
        mapper(mpr_device, &recorder, &sequencer) {}

  void update() {
    mapper.update();
    sequencer.update();
    midiOut.flush();
  }

  MidiOut midiOut;
  Pattern pattern;
  Sequencer sequencer;
  GestureRecorder recorder;
  Mapper mapper;
};
}  // namespace MapLooper
