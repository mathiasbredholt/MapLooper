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
#include "MapLooper/Signal.hpp"
#include "MapLooper/Track.hpp"

namespace MapLooper {
class Scene {
 public:
  uint8_t id{0};
  uint8_t activeTrackID{0};

  Scene() { activeTrackID = 0; }

  void update(Tick tick, const SignalMap& signalMap) {
    for (Track& t : tracks) {
      if (t.getEnabled()) {
        t.update(tick, signalMap);
      }
    }
  }

  Track& getActiveTrack() { return tracks[activeTrackID]; }

  void setActiveTrack(int id) { activeTrackID = id; }

  static Track* trackToCopy;

  std::vector<Track> tracks;
};
}  // namespace MapLooper
