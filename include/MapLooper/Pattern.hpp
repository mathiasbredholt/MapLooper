/*
 MapLooper - Embedded Live-Looping Tools for Digital Musical Instruments
 Copyright (C) 2020 Mathias Bredholt
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>

#include "MapLooper/Clock.hpp"
#include "MapLooper/SignalInfo.hpp"
#include "MapLooper/Track.hpp"

class Bank;
class PatternController;
class Torso;

namespace MapLooper {
class Pattern {
 public:
  uint8_t id{0};

  uint8_t activeTrackID{0};

  Pattern(MidiOut* midiOut) : _midiOut(midiOut) { clear(); }

  void update(Tick tick, const SignalInfoMap& signalInfoMap) {
    if (tick % getLength() == 0) {
      resetTo(tick / getLength() * getLength());
    }
    for (Track& t : tracks) {
      if (t.getEnabled()) {
        t.update(tick - resetPoint, getLength(), signalInfoMap);
      }
    }
  }

  void resetTo(int32_t time_point) {
    resetPoint = time_point;
    for (Track& r : tracks) {
      r.releaseNotes();
    }
  }

  void clear() {
    length = 384;
    for (Track& r : tracks) {
      r.clear();
    }
    activeTrackID = 0;
  }

  void releaseAll() {
    for (Track& r : tracks) {
      r.releaseNotes();
    }
  }

  Track& getActiveTrack() { return tracks[activeTrackID]; }

  void setActiveTrack(int id) { activeTrackID = id; }

  int32_t getLocalTicks(int32_t ticks) const { return ticks - resetPoint; }

  Tick getLength() { return length; }

  static Track* trackToCopy;

  Tick resetPoint{0};

  std::vector<Track> tracks;

  int length = {384};

  MidiOut* _midiOut;
};
}  // namespace MapLooper
