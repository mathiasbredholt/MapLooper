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

#include "MapLooper/Clock.hpp"
#include "MapLooper/Loop.hpp"
// #include "MapLooper/Sequencer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mapper/mapper.h"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper() : dev(mpr_dev_new("MapLooper", 0)) {
    while (!mpr_dev_get_is_ready(dev)) {
      mpr_dev_poll(dev, 10);
    }
    mpr_graph_subscribe(mpr_obj_get_graph(dev), nullptr, MPR_SIG, -1);
  }

  Loop* createLoop(const char* id, mpr_type type, int length, const char* src,
                   const char* dst) {
    Loop* loop = new Loop(id, dev, type, length, src, dst);
    loops.push_back(loop);
    return loop;
  }

  void update() {
    mpr_dev_poll(dev, 0);
    int t = _clock.getTicks();
    if (lastUpdate != t) {
      lastUpdate = t;
      for (auto& l : loops) {
        l->update();
      }
    }
  }

  mpr_dev getMapperDevice() { return dev; }

 private:
  std::vector<Loop*> loops;
  Clock _clock;
  mpr_dev dev;

  int lastUpdate = 0;
};
}  // namespace MapLooper
