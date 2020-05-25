/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This progrxam is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

#include <iostream>

#include "MapLooper/Pattern.hpp"
#include "MapLooper/Sequencer.hpp"
#include "MapLooper/TickTimer.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "MapLooper/mpr/Mapper.hpp"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper() { Mapper::getInstance().setSequencer(&sequencer); }

  Mapper& getMapper() { return Mapper::getInstance(); }

 private:
  Sequencer sequencer;
};
}  // namespace MapLooper
