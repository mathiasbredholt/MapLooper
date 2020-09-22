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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mapper/mapper.h"

namespace MapLooper {
class Loop {
 public:
  Loop(const char* id, mpr_dev dev, mpr_type type, int length, const char* src,
       const char* dst)
      : _id(id), _type(type), _length(length), _src(src), _dst(dst) {
    char sigName[32];

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "in");
    sigLoopIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, 0, 0, 0,
                            sigLoopInHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigLoopIn, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "out");
    sigLoopOut = mpr_sig_new(dev, MPR_DIR_OUT, sigName, 1, MPR_FLT, NULL, NULL,
                             NULL, NULL, NULL, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "mix");
    sigMix =
        mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, 0, 0, 0, 0, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "modulation");
    sigMod =
        mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, 0, 0, 0, 0, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "mute");
    sigMute = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_INT32, 0, 0, 0, 0,
                          sigMuteHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigMute, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "local/out");
    sigLocalOut =
        mpr_sig_new(dev, MPR_DIR_OUT, sigName, 1, MPR_FLT, 0, 0, 0, 0, 0, 0);

    snprintf(sigName, sizeof(sigName), "%s/%s", id, "local/in");
    sigLocalIn = mpr_sig_new(dev, MPR_DIR_IN, sigName, 1, MPR_FLT, 0, 0, 0, 0,
                             sigLocalInHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop(sigLocalIn, MPR_PROP_DATA, 0, 1, MPR_PTR, this, 0);

    graph = mpr_obj_get_graph(dev);

    // Automap handler
    _ioMapHandler = [](mpr_graph g, mpr_obj obj, const mpr_graph_evt evt,
                       const void* data) {
      const char* sigName = mpr_obj_get_prop_as_str(obj, MPR_PROP_NAME, 0);
      Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(data));
      if (strcmp(loop->_src, sigName) == 0) {
        loop->inMap = mpr_map_new(1, &obj, 1, &loop->sigLoopIn);
        mpr_obj_push(loop->inMap);
        printf("Found input, creating map!\n");
      } else if (strcmp(loop->_dst, sigName) == 0) {
        loop->outMap = mpr_map_new(1, &loop->sigLoopOut, 1, &obj);
        mpr_obj_push(loop->outMap);
        printf("Found output, creating map!\n");
      }
    };
    mpr_graph_add_cb(graph, _ioMapHandler, MPR_SIG, this);

    size_t typeSize;
    switch (_type) {
      case MPR_INT32:
        typeSize = sizeof(int);
        break;
      case MPR_FLT:
        typeSize = sizeof(float);
        break;
      default:
        typeSize = sizeof(double);
        break;
    }

    buffer = std::malloc(typeSize * _length);
    output = std::malloc(typeSize * _length);
    std::memset(buffer, 0, typeSize * _length);
    std::memset(output, 0, typeSize * _length);

    mpr_sig sigs[] = {sigLocalOut, sigMix, sigMod};
    map = mpr_map_new(3, sigs, 1, &sigLocalIn);

    // Update delay length and set map expression
    setDelayLength(384);
  }

  void setDelayLength(int value) {
    if (value > 768) {
      value = 768;
      printf("Delay lengths should be <= 768\n");
    }

    delayLength = value;

    // Generate map expression
    char expr[128];
    snprintf(expr, sizeof(expr),
             "y=((1-x1)*x0+x1*y{-%d})*((1-x2)+x2*uniform(1.0))", delayLength);

    // Set expression and push map
    mpr_obj_set_prop(map, MPR_PROP_EXPR, 0, 1, MPR_STR, expr, 1);
    mpr_obj_push(map);
  }

  void mapMixTo(const char* sig) {
    _mix = sig;
    mpr_graph_handler* mixMapHAndler = [](mpr_graph g, mpr_obj obj,
                                           const mpr_graph_evt evt,
                                           const void* data) {
      const char* sigName = mpr_obj_get_prop_as_str(obj, MPR_PROP_NAME, 0);
      Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(data));
      if (strcmp(loop->_mix, sigName) == 0) {
        mpr_obj_push(mpr_map_new(1, &obj, 1, &loop->sigMix));
      }
    };
    mpr_graph_add_cb(graph, mixMapHAndler, MPR_SIG, this);
  }

  mpr_map getMap() { return map; }

  ~Loop() {
    free(buffer);
    free(output);
    mpr_sig_free(sigLoopIn);
    mpr_sig_free(sigLoopOut);
    mpr_sig_free(sigMix);
    mpr_sig_free(sigLocalOut);
    mpr_sig_free(sigLocalIn);
  }

  void update() {
    mpr_sig_set_value(sigLocalOut, 0, _length, _type, buffer);
    mpr_sig_set_value(sigLoopOut, 0, _length, _type, output);
  }

  static void sigLoopInHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                               int length, mpr_type type, const void* value,
                               mpr_time time) {
    Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(
        mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    switch (type) {
      case MPR_INT32:
        std::memcpy(loop->buffer, value, sizeof(int) * length);
        break;
      case MPR_FLT:
        std::memcpy(loop->buffer, value, sizeof(float) * length);
        break;
      default:
        std::memcpy(loop->buffer, value, sizeof(double) * length);
        break;
    }
    // printf("buffer: %f\n", *((float*) loop->buffer));
  }

  static void sigLocalInHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                int length, mpr_type type, const void* value,
                                mpr_time time) {
    Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(
        mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    switch (type) {
      case MPR_INT32:
        std::memcpy(loop->output, value, sizeof(int) * length);
        break;
      case MPR_FLT:
        std::memcpy(loop->output, value, sizeof(float) * length);
        break;
      default:
        std::memcpy(loop->output, value, sizeof(double) * length);
        break;
    }
    // printf("out: %f\n", *((float*) loop->output));
  }

  static void sigMuteHandler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                             int length, mpr_type type, const void* value,
                             mpr_time time) {
    Loop* loop = const_cast<Loop*>(reinterpret_cast<const Loop*>(
        mpr_obj_get_prop_as_ptr(sig, MPR_PROP_DATA, 0)));
    const bool val = *reinterpret_cast<const int*>(value);
    mpr_obj_set_prop(loop->outMap, MPR_PROP_MUTED, 0, 1, MPR_BOOL, &val, 1);
  }

  mpr_sig sigLoopIn;
  mpr_sig sigLoopOut;
  mpr_sig sigMix;
  mpr_sig sigMod;
  mpr_sig sigLocalOut;
  mpr_sig sigLocalIn;
  mpr_sig sigMute;
  mpr_map map;
  mpr_map outMap = 0;
  mpr_map inMap = 0;
  mpr_graph graph;

  void* buffer;
  void* output;

 private:
  const char* _id;
  mpr_type _type;
  int _length;

  mpr_graph_handler* _ioMapHandler;

  const char* _src;
  const char* _dst;
  const char* _mix;

  int delayLength;
};
}  // namespace MapLooper
