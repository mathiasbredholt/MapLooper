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

#include <vector>

#include "MapLooper/Loop.hpp"
#include "ableton/Link.hpp"
#include "mapper/mapper.h"

namespace MapLooper {
class MapLooper {
 public:
  MapLooper() : _dev(mpr_dev_new("MapLooper", 0)), _link(120.0) {
    // Start Ableton Link
    _link.enable(true);
    _link.enableStartStopSync(true);

    // Poll device until ready
    while (!mpr_dev_get_is_ready(_dev)) {
      mpr_dev_poll(_dev, 10);
    }

    // Refresh all stale maps
    mpr_list maps = mpr_graph_get_objs(mpr_obj_get_graph(_dev), MPR_MAP);
    while (maps) {
      mpr_map_refresh(*maps);
      maps = mpr_list_get_next(maps);
    }

    // Subscribe to all signals for automapping
    mpr_graph_subscribe(mpr_obj_get_graph(_dev), nullptr, MPR_SIG, -1);
  }

  Loop* createLoop(const char* id, mpr_type type = MPR_FLT,
                   int vectorSize = 1) {
    Loop* loop = new Loop(id, _dev, type, vectorSize);
    _loops.push_back(loop);
    return loop;
  }

  void update(int blockMs) {
    // Poll libmapper device
    mpr_dev_poll(_dev, blockMs);

    // Get beats from Link session
    auto state = _link.captureAudioSessionState();
    double beats = state.beatAtTime(_link.clock().micros(), 4.0);

    for (auto& l : _loops) {
      l->update(beats);
    }
  }

  mpr_dev getDevice() { return _dev; }

 private:
  mpr_dev _dev;
  ableton::Link _link;
  std::vector<Loop*> _loops;
};
}  // namespace MapLooper
