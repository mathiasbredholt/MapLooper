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
    graph = mpr_graph_new(0);
    dev = mpr_dev_new("MapLooper", graph);
    _lookForInterestingSignals();

    int sigRecordMin = 0, sigRecordMax = 1;
    recSignal = mpr_sig_new(
        dev, MPR_DIR_IN, "/control/record", 1, MPR_INT32, 0, &sigRecordMin,
        &sigRecordMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setRecording(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);
    mpr_obj_set_prop(recSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR, this, 0);

    int sigTrackSelectMin = 0, sigTrackSelectMax = NUM_TRACKS - 1;
    trackSelectSignal = mpr_sig_new(
        dev, MPR_DIR_IN, "/control/trackSelect", 1, MPR_INT32, 0,
        &sigTrackSelectMin, &sigTrackSelectMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setActiveTrack(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);
    mpr_obj_set_prop(trackSelectSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR, this,
                     0);

    int playStateMin = 0, playStateMax = 1;
    playStateSignal = mpr_sig_new(
        dev, MPR_DIR_IN, "/control/playState", 1, MPR_INT32, 0, &playStateMin,
        &playStateMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setPlayState(*static_cast<const int*>(value));
        },
        MPR_SIG_UPDATE);
    mpr_obj_set_prop(playStateSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR, this, 0);

    float lengthMin = 1, lengthMax = 768;
    lengthSignal = mpr_sig_new(
        dev, MPR_DIR_IN, "/control/length", 1, MPR_FLT, 0, &lengthMin,
        &lengthMax, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          MapLooper* mapLooper = (MapLooper*)mpr_obj_get_prop_as_ptr(
              (mpr_obj)sig, MPR_PROP_DATA, 0);
          mapLooper->_sequencer.setLength(*static_cast<const float*>(value));
        },
        0);
    mpr_obj_set_prop(lengthSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR, this, 0);

    xTaskCreate(
        [](void* userParam) {
          MapLooper* mapLooper = static_cast<MapLooper*>(userParam);
          for (;;) {
            mpr_dev_poll(mapLooper->dev, 0);
            vTaskDelay(1);
          }
        },
        "mapper", 16384, this, 3, nullptr);
  }

  void addSignal(const std::string& path, float min, float max,
                 Signal::Callback signalCallback) {
    _sequencer.addSignal(path, min, max, signalCallback, dev);
  }

  mpr_dev getMapperDevice() { return dev; }

  Sequencer& getSequencer() { return _sequencer; }

  void autoMap() {
    while (!mpr_dev_get_is_ready(dev)) {
      mpr_dev_poll(dev, 0);
      vTaskDelay(100);
    }
    ESP_LOGI(_getTag(), "Device ready");
    mpr_graph_subscribe(graph, nullptr, MPR_SIG, -1);
  }

 private:
  static const char* _getTag() { return "MapLooper"; }

  void _lookForInterestingSignals() {
    mpr_graph_add_cb(
        graph,
        [](mpr_graph g, mpr_obj obj, const mpr_graph_evt evt,
           const void* data) {
          MapLooper* mapLooper = (MapLooper*)data;

          std::string name =
              mpr_obj_get_prop_as_str((mpr_sig)obj, MPR_PROP_NAME, nullptr);

          ESP_LOGI(_getTag(), "Found signal:");
          mpr_obj_print(obj, 0);

          mpr_list sigs = mpr_dev_get_sigs(mapLooper->dev, MPR_DIR_IN);

          if (name == "slider1") {
            ESP_LOGI(_getTag(), "Found slider1, creating map...");
            // sigs = mpr_list_filter(sigs, MPR_PROP_NAME, nullptr, 1, MPR_STR,
            //                        "pressure", MPR_OP_EQ);
            while (sigs) {
              std::string name =
                  mpr_obj_get_prop_as_str(*sigs, MPR_PROP_NAME, nullptr);
              if (name == "pressure") {
                mpr_map map = mpr_map_new(1, (mpr_sig*)&obj, 1, sigs);
                mpr_obj_push(map);
                break;
              }
              sigs = mpr_list_get_next(sigs);
            }
          } else if (name == "slider2") {
            ESP_LOGI(_getTag(), "Found slider2, creating map...");
            // sigs = mpr_list_filter(sigs, MPR_PROP_NAME, nullptr, 1, MPR_STR,
            //                        "tubeLength", MPR_OP_EQ);
            while (sigs) {
              std::string name =
                  mpr_obj_get_prop_as_str(*sigs, MPR_PROP_NAME, nullptr);
              if (name == "tubeLength") {
                mpr_map map = mpr_map_new(1, (mpr_sig*)&obj, 1, sigs);
                mpr_obj_push(map);

                break;
              }
              sigs = mpr_list_get_next(sigs);
            }
          } else if (name == "button1") {
            ESP_LOGI(_getTag(), "Found button1, creating map...");
            mpr_map map =
                mpr_map_new(1, (mpr_sig*)&obj, 1, &mapLooper->recSignal);
            mpr_obj_push(map);
          }
        },
        MPR_SIG, this);
  }

  mpr_dev dev;
  mpr_graph graph;
  Sequencer _sequencer;
  mpr_sig lengthSignal;
  mpr_sig recSignal;
  mpr_sig trackSelectSignal;
  mpr_sig playStateSignal;
};
}  // namespace MapLooper
