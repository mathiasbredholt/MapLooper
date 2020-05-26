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

#include <unordered_map>
#include <vector>

#include "MapLooper/Sequencer.hpp"
#include "MapLooper/Util.hpp"
#include "esp_log.h"
#include "mpr/mpr.h"

namespace MapLooper {
class Mapper {
 public:
  inline static Mapper& getInstance() {
    static Mapper mapper;
    return mapper;
  }

  Mapper() {
    dev = mpr_dev_new("MapLooper", 0);

    int sigRecordMin = 0, sigRecordMax = 1;
    mpr_sig_new(
        dev, MPR_DIR_IN, "/control/record", 1, MPR_INT32, 0, &sigRecordMin,
        &sigRecordMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          getInstance()._sequencer->setRecording(
              *static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    int sigTrackSelectMin = 0, sigTrackSelectMax = NUM_TRACKS - 1;
    mpr_sig_new(
        dev, MPR_DIR_IN, "/control/trackSelect", 1, MPR_INT32, 0,
        &sigTrackSelectMin, &sigTrackSelectMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          getInstance()._sequencer->setActiveTrack(
              *static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    int playStateMin = 0, playStateMax = 1;
    mpr_sig_new(
        dev, MPR_DIR_IN, "/control/playState", 1, MPR_INT32, 0, &playStateMin,
        &playStateMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          getInstance()._sequencer->setPlayState(
              *static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    xTaskCreate(
        [](void* userParam) {
          for (;;) {
            getInstance().update();
            portYIELD();
          }
        },
        "mapper", 4096, nullptr, 3, nullptr);
  }

  void update() { mpr_dev_poll(dev, 0); }

  void addSignal(const std::string& path, float min, float max,
                 SignalCallback signalCallback) {
    const Signal signal(signalCallback, min, max);
    mpr_sig_new(
        dev, MPR_DIR_IN, path.c_str(), 1, MPR_FLT, 0, &min, &max, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          getInstance()._sequencer->setValue(
              mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL),
              *static_cast<const float*>(value));
        },
        MPR_SIG_UPDATE);
    _sequencer->addSignal(path, signal);
  }

  void setSequencer(Sequencer* sequencer) { _sequencer = sequencer; }

 private:
  Sequencer* _sequencer;

  mpr_dev dev;
};
}  // namespace MapLooper
