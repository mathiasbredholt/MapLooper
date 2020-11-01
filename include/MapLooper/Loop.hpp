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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "mapper/mapper.h"

namespace MapLooper {
class Loop {
 public:
  Loop(const char* name, mpr_dev dev, mpr_type type, int vectorSize)
      : _graph(mpr_obj_get_graph(dev)), _type(type), _vectorSize(vectorSize) {
    char sigName[128];
    int muteMin = 0, muteMax = 1;
    float sigMin = 0.0f, sigMax = 1.0f;
    float minDelay = -100.0f, maxDelay = -1.0f;

    // Create control signals

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "record");
    sigRecord = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                            &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "delay");
    sigDelay = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &minDelay,
                           &maxDelay, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "modulation");
    sigMod = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                         &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "mute");
    sigMute = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_INT32, 0, &muteMin,
                          &muteMax, 0, 0, 0);
    mpr_sig_set_value(sigMute, 0, 1, MPR_INT32, &muteMin);

    // Create input and output signals

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "input");
    sigIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, _vectorSize, _type, 0,
                        &sigMin, &sigMax, 0, 0, 0);
    mpr_sig_set_value(sigIn, 0, _vectorSize, _type, &sigMin);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "output");
    sigOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, _vectorSize, _type, 0,
                         &sigMin, &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/out");
    sigLocalOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, _vectorSize, _type, 0,
                              &sigMin, &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/in");
    sigLocalIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, _vectorSize, _type, 0,
                             &sigMin, &sigMax, 0, 0, 0);

    // Create map

    loopMap = mpr_map_new_from_str(
        "%y=(_%x*%x+(1-_%x)*y{_%x,100})*((1-_%x)+_%x*uniform(2.0))", sigLocalIn,
        sigRecord, sigLocalOut, sigRecord, sigDelay, sigMod, sigMod);
    mpr_obj_push(loopMap);

    while (!mpr_map_get_is_ready(loopMap)) {
      mpr_dev_poll(dev, 10);
    }

    setLength(1.0);
  }

  ~Loop() {
    mpr_sig_free(sigRecord);
    mpr_sig_free(sigDelay);
    mpr_sig_free(sigMod);
    mpr_sig_free(sigIn);
    mpr_sig_free(sigOut);
    mpr_sig_free(sigLocalOut);
    mpr_sig_free(sigLocalIn);
  }

  void setMute(bool value) {
    mpr_obj_set_prop(outMap, MPR_PROP_MUTED, 0, 1, MPR_INT32, &value, 0);
  }

  void mapRecord(const char* src) { _mapFrom(src, &sigRecord); }

  void mapDelay(const char* src) { _mapFrom(src, &sigDelay); }

  void mapModulation(const char* src) { _mapFrom(src, &sigMod); }

  void mapInput(const char* src) { _mapFrom(src, &sigIn); }

  void mapOutput(const char* dst) { _mapTo(&sigOut, dst); }

  void update(double beats) {
    int now = beats * _ppqn;
    if (now != _lastUpdate) {
      // Check if ticks were missed
      if (now - _lastUpdate > 1) {
        printf("Missed %d ticks!\n", now - _lastUpdate - 1);
      }

      // Update local out
      const void* inputValue = mpr_sig_get_value(sigIn, 0, 0);
      mpr_sig_set_value(sigLocalOut, 0, _vectorSize, _type, inputValue);

      // Check if muted
      bool muted = *((int*)mpr_sig_get_value(sigMute, 0, 0));

      if (!muted) {
        // Update output
        const void* outputValue = mpr_sig_get_value(sigLocalIn, 0, 0);
        mpr_sig_set_value(sigOut, 0, _vectorSize, _type, outputValue);
      }

      _lastUpdate = now;
    }
  }

  int getPulsesPerQuarterNote() { return _ppqn; }

  void setPulsesPerQuarterNote(int value) {
    _ppqn = value;

    // PPQN changed, length needs to be updated
    setLength(_length);
  }

  float getLength() { return _length; }

  void setLength(float beats) {
    _length = beats;
    float delay = -_ppqn * _length;
    mpr_sig_set_value(sigDelay, 0, 1, MPR_FLT, &delay);
  }

  mpr_sig sigRecord;
  mpr_sig sigDelay;
  mpr_sig sigMod;
  mpr_sig sigIn;
  mpr_sig sigOut;
  mpr_sig sigLocalOut;
  mpr_sig sigLocalIn;
  mpr_sig sigMute;
  mpr_map loopMap;
  mpr_map outMap = 0;
  mpr_map inMap = 0;

 private:
  void _mapFrom(const char* src, mpr_sig* dst) {
    struct MapData {
      const char* src;
      mpr_sig* dst;
    };
    MapData* mapData = new MapData{src, dst};

    mpr_graph_handler* handler = [](mpr_graph g, mpr_obj obj,
                                    const mpr_graph_evt evt, const void* data) {
      MapData* mapData = reinterpret_cast<MapData*>(const_cast<void*>(data));
      const char* found = mpr_obj_get_prop_as_str(obj, MPR_PROP_NAME, 0);
      if (strcmp(mapData->src, found) == 0) {
        mpr_obj_push(mpr_map_new(1, (mpr_sig*)&obj, 1, mapData->dst));
      }
    };
    mpr_graph_add_cb(_graph, handler, MPR_SIG, mapData);
  }

  void _mapTo(mpr_sig* src, const char* dst) {
    struct MapData {
      mpr_sig* src;
      const char* dst;
    };
    MapData* mapData = new MapData{src, dst};

    mpr_graph_handler* handler = [](mpr_graph g, mpr_obj obj,
                                    const mpr_graph_evt evt, const void* data) {
      MapData* mapData = reinterpret_cast<MapData*>(const_cast<void*>(data));
      const char* found = mpr_obj_get_prop_as_str(obj, MPR_PROP_NAME, 0);
      if (strcmp(mapData->dst, found) == 0) {
        mpr_obj_push(mpr_map_new(1, mapData->src, 1, (mpr_sig*)&obj));
      }
    };
    mpr_graph_add_cb(_graph, handler, MPR_SIG, mapData);
  }

  mpr_graph _graph;

  int _ppqn = 16;
  int _lastUpdate = 0;

  mpr_type _type;
  int _vectorSize;
  float _length;
};
}  // namespace MapLooper
