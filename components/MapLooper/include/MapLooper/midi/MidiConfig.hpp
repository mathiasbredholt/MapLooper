/*
           __  
  /\/\    / /  
 /    \  / /   
/ /\/\ \/ /___ 
\/    \/\____/ 
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

MidiConfig.hpp
Midi configuration 

*/

#pragma once

#ifdef BLEMIDI
#include "MapLooper/midi/BLEMidiDevice.hpp"
#else
#include "MapLooper/midi/LocalMidiDevice.hpp"
#endif

namespace MapLooper {
#ifdef BLEMIDI
using MidiOut = BLEMidiDevice;
#else
using MidiOut = LocalMidiDevice;
#endif
}  // namespace MapLooper
