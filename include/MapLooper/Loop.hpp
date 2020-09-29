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
  mpr_sig sigIn;
  mpr_sig sigOut;
  mpr_sig sigMix;
  mpr_sig sigMod;
  mpr_sig sigLocalOut;
  mpr_sig sigLocalIn;
  mpr_sig sigMute;
  mpr_map map;
  mpr_map outMap = 0;
  mpr_map inMap = 0;
  mpr_graph graph;

  Loop(const char* name, mpr_dev dev, mpr_type type, int length)
      : _name(name), _type(type), _length(length) {
    // Create loop signals
    char sigName[128];
    float sigMin = 0.0f, sigMax = 1.0f;

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "in");
    sigIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                        &sigMax, 0, _sigInHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigIn, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "out");
    sigOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, 1, MPR_FLT, 0, &sigMin,
                         &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "mix");
    sigMix = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                         &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "modulation");
    sigMod = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                         &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "mute");
    sigMute = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_INT32, 0, 0, 0, 0,
                          _sigMuteHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigMute, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/out");
    sigLocalOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, 1, MPR_FLT, 0, &sigMin,
                              &sigMax, 0, 0, 0);

    std::snprintf(sigName, sizeof(sigName), "%s/%s", name, "local/in");
    sigLocalIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, &sigMin,
                             &sigMax, 0, _sigLocalInHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigLocalIn, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    graph = mpr_obj_get_graph(dev);

    switch (_type) {
      case MPR_INT32:
        _frameSize = sizeof(int) * _length;
        break;
      case MPR_FLT:
        _frameSize = sizeof(float) * _length;
        break;
      default:
        _frameSize = sizeof(double) * _length;
        break;
    }

    buffer = std::malloc(_frameSize);
    output = std::malloc(_frameSize);
    std::memset(buffer, 0, _frameSize);
    std::memset(output, 0, _frameSize);

    mpr_sig sigs[] = {sigLocalOut, sigMix, sigMod};
    map = mpr_map_new(3, sigs, 1, &sigLocalIn);

    // Update buffer size and set map expression
    setBufferSize(16);
  }

  ~Loop() {
    free(buffer);
    free(output);
    mpr_sig_free(sigIn);
    mpr_sig_free(sigOut);
    mpr_sig_free(sigMix);
    mpr_sig_free(sigLocalOut);
    mpr_sig_free(sigLocalIn);
  }

  void setBufferSize(int value) {
    if (value > 768) {
      value = 768;
      std::printf("Buffer lengths should be <= 768\n");
    }

    bufferSize = value;

    // Generate map expression
    char expr[128];
    std::snprintf(expr, sizeof(expr),
                  "y=((1-_x1)*x0+_x1*y{-%d})*((1-_x2)+_x2*uniform(2.0))",
                  bufferSize);

    // Set expression and push map
    mpr_obj_set_prop(map, MPR_PROP_EXPR, 0, 1, MPR_STR, expr, 1);
    mpr_obj_push(map);
  }

  void setMute(bool value) {
    mpr_obj_set_prop(outMap, MPR_PROP_MUTED, 0, 1, MPR_INT32, &value, 0);
  }

  void mapMix(const char* src) { _mapFrom(src, &sigMix); }

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

      // Update buffer and output
      mpr_sig_set_value(sigLocalOut, 0, _length, _type, buffer);
      mpr_sig_set_value(sigOut, 0, _length, _type, output);

      _lastUpdate = now;
    }
  }

  void setPulsesPerQuarterNote(int value) { _ppqn = value; }

 private:
  static void _sigInHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                            int length, mpr_type type, const void* value,
                            mpr_time time) {
    Loop* loop = reinterpret_cast<Loop*>(
        const_cast<void*>(mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    std::memcpy(loop->buffer, value, loop->_frameSize);
  }

  static void _sigLocalInHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                 int length, mpr_type type, const void* value,
                                 mpr_time time) {
    Loop* loop = reinterpret_cast<Loop*>(
        const_cast<void*>(mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    std::memcpy(loop->output, value, loop->_frameSize);
  }

  static void _sigMuteHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                              int length, mpr_type type, const void* value,
                              mpr_time time) {
    Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(
        mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    const bool val = *reinterpret_cast<const int*>(value);
    mpr_obj_set_prop(loop->outMap, MPR_PROP_MUTED, 0, 1, MPR_BOOL, &val, 1);
  }

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
        mpr_obj_push(mpr_map_new(1, &obj, 1, mapData->dst));
      }
    };
    mpr_graph_add_cb(graph, handler, MPR_SIG, mapData);
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
        mpr_obj_push(mpr_map_new(1, mapData->src, 1, &obj));
      }
    };
    mpr_graph_add_cb(graph, handler, MPR_SIG, mapData);
  }

  int _ppqn = 4;
  int _lastUpdate = 0;

  void* buffer;
  void* output;

  const char* _name;
  mpr_type _type;
  size_t _frameSize;
  int _length;
  int bufferSize;
};
}  // namespace MapLooper
