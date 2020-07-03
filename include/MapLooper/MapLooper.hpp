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

#include "MapLooper/Sequencer.hpp"
#include "mpr/mpr.h"

namespace MapLooper {

class MapLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  MapLooper() {
    ESP_LOGI(_getTag(), "Creating device...");
    dev = mpr_dev_new("MapLooper", 0);

    int sigRecordMin = 0, sigRecordMax = 1;
    _createSignal(
        dev, MPR_DIR_IN, "/control/record", 1, MPR_INT32, 0, &sigRecordMin,
        &sigRecordMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setRecording(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    int sigTrackSelectMin = 0, sigTrackSelectMax = NUM_TRACKS - 1;
    _createSignal(
        dev, MPR_DIR_IN, "/control/trackSelect", 1, MPR_INT32, 0,
        &sigTrackSelectMin, &sigTrackSelectMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setActiveTrack(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    int playStateMin = 0, playStateMax = 1;
    _createSignal(
        dev, MPR_DIR_IN, "/control/playState", 1, MPR_INT32, 0, &playStateMin,
        &playStateMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setPlayState(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);

    float lengthMin = 1, lengthMax = 768;
    _createSignal(
        dev, MPR_DIR_IN, "/control/length", 1, MPR_FLT, 0, &lengthMin,
        &lengthMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setLength(*static_cast<const float*>(value));
        },
        MPR_SIG_UPDATE);

    xTaskCreatePinnedToCore(
        [](void* userParam) {
          MapLooper* mapLooper = static_cast<MapLooper*>(userParam);
          for (;;) {
            mpr_dev_poll(mapLooper->dev, 0);
            vTaskDelay(1);
          }
        },
        "mapper", 16384, this, 3, nullptr, 1);
  }

  void addSignal(const std::string& path, float min, float max,
                 Signal::Callback signalCallback) {
    _sequencer.addSignal(path, min, max, signalCallback, dev);
  }

  mpr_dev getMapperDevice() { return dev; }

  Sequencer& getSequencer() { return _sequencer; }

 private:
  static const char* _getTag() { return "MapLooper"; }

  void _createSignal(mpr_dev parent, mpr_dir dir, const char* name, int len,
                     mpr_type type, const char* unit, const void* min,
                     const void* max, int* num_inst, mpr_sig_handler* handler,
                     int events) {
    mpr_sig newSignal = mpr_sig_new(parent, dir, name, len, type, unit, min,
                                    max, num_inst, handler, events);
    mpr_obj_set_prop((mpr_obj)newSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR, this,
                     0);
  }

  mpr_dev dev;
  Sequencer _sequencer;
  std::unordered_map<std::string, mpr_sig> _outputSignals;
};
}  // namespace MapLooper
