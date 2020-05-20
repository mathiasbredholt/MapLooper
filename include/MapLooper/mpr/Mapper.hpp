/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Mapper.hpp
libmapper interface

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

    while (!mpr_dev_get_is_ready(dev)) {
      mpr_dev_poll(dev, 25);
    }
  }

  void update() { mpr_dev_poll(dev, 0); }

  void addSignal(const std::string& path, float min, float max,
                 SignalCallback signalCallback) {
    const SignalInfo signalInfo(signalCallback, min, max);
    mpr_sig_new(
        dev, MPR_DIR_IN, path.c_str(), 1, MPR_FLT, 0, &min, &max, 0,
        [](mpr_sig sig, mpr_sig_evt evt, mpr_id inst, int length, mpr_type type,
           const void* value, mpr_time time) {
          getInstance()._sequencer->setValue(
              mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL),
              *static_cast<const float*>(value));
        },
        MPR_SIG_UPDATE);
    _sequencer->addSignal(path, signalInfo);
  }

  void setSequencer(Sequencer* sequencer) { _sequencer = sequencer; }

 private:
  Sequencer* _sequencer;

  mpr_dev dev;
};
}  // namespace MapLooper
