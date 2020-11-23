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
#include <type_traits>

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
    _sigRecord = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                             &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "delay");
    _sigDelay = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &minDelay,
                            &maxDelay, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "modulation");
    _sigModulation = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                          &sigMax, 0, 0, 0);
    mpr_sig_set_value(_sigModulation, 0, 1, MPR_FLT, &sigMin);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "mute");
    _sigMute = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_INT32, 0, &muteMin,
                           &muteMax, 0, 0, 0);
    mpr_sig_set_value(_sigMute, 0, 1, MPR_INT32, &muteMin);

    // Create input and output signals

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "input");
    _sigIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, _vectorSize, _type, 0,
                         &sigMin, &sigMax, 0, 0, 0);
    mpr_sig_set_value(_sigIn, 0, _vectorSize, _type, &sigMin);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "output");
    _sigOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, _vectorSize, _type, 0,
                          &sigMin, &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/out");
    _sigLocalOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, _vectorSize, _type, 0,
                               &sigMin, &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/in");
    _sigLocalIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, _vectorSize, _type, 0,
                              &sigMin, &sigMax, 0, 0, 0);

    // Create map

    _loopMap = mpr_map_new_from_str(
        "%y=_%x*%x+(1-_%x)*y{_%x,100}+_%x*(uniform(2.0)-1)",
        _sigLocalIn, _sigRecord, _sigLocalOut, _sigRecord, _sigDelay, _sigModulation,
        _sigModulation);
    mpr_obj_push(_loopMap);

    while (!mpr_map_get_is_ready(_loopMap)) {
      mpr_dev_poll(dev, 10);
    }

    setLength(1.0);
  }

  ~Loop() {
    mpr_sig_free(_sigRecord);
    mpr_sig_free(_sigDelay);
    mpr_sig_free(_sigModulation);
    mpr_sig_free(_sigIn);
    mpr_sig_free(_sigOut);
    mpr_sig_free(_sigLocalOut);
    mpr_sig_free(_sigLocalIn);
  }

  void mapRecord(const char* src) { _mapFrom(src, &_sigRecord); }

  void mapDelay(const char* src) { _mapFrom(src, &_sigDelay); }

  void mapModulation(const char* src) { _mapFrom(src, &_sigModulation); }

  void mapInput(const char* src) { _mapFrom(src, &_sigIn); }

  void mapOutput(const char* dst) { _mapTo(&_sigOut, dst); }

  void update(double beats) {
    int now = beats * _ppqn;
    if (now != _lastUpdate) {
      // Check if ticks were missed
      if (now - _lastUpdate > 1) {
        printf("Missed %d ticks!\n", now - _lastUpdate - 1);
      }

      // Update local out
      const void* inputValue = mpr_sig_get_value(_sigIn, 0, 0);
      mpr_sig_set_value(_sigLocalOut, 0, _vectorSize, _type, inputValue);

      // Check if muted
      bool muted = *((int*)mpr_sig_get_value(_sigMute, 0, 0));

      if (!muted) {
        // Update output
        const void* outputValue = mpr_sig_get_value(_sigLocalIn, 0, 0);
        mpr_sig_set_value(_sigOut, 0, _vectorSize, _type, outputValue);
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

  int getNumSamples() { return _ppqn * _length; }

  float getLength() { return _length; }

  void setLength(float beats) {
    _length = beats;
    float delay = -_ppqn * _length;
    mpr_sig_set_value(_sigDelay, 0, 1, MPR_FLT, &delay);
  }

  mpr_sig getInputSignal() { return _sigIn; }

  mpr_sig getOutputSignal() { return _sigOut; }

  mpr_sig getModulationSignal() { return _sigModulation; }

  mpr_sig getDelaySignal() { return _sigDelay; }

  mpr_sig getRecordSignal() { return _sigRecord; }

  mpr_sig getMuteSignal() { return _sigMute; }

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
  mpr_map _loopMap;
  mpr_sig _sigRecord;
  mpr_sig _sigDelay;
  mpr_sig _sigModulation;
  mpr_sig _sigIn;
  mpr_sig _sigOut;
  mpr_sig _sigLocalOut;
  mpr_sig _sigLocalIn;
  mpr_sig _sigMute;

  int _ppqn = 16;
  float _length;
  int _lastUpdate = 0;

  mpr_type _type;
  int _vectorSize;
};
}  // namespace MapLooper
