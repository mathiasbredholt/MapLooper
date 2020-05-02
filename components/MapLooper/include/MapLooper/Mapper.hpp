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

#include "MapLooper/GestureRecorder.hpp"
#include "MapLooper/Sequencer.hpp"
#include "MapLooper/Util.hpp"
#include "mpr/mpr.h"

namespace MapLooper {
class Mapper {
 public:
  Mapper(mpr_dev* dev, GestureRecorder* recorder, Sequencer* sequencer)
      : _dev(dev), _recorder(recorder), _sequencer(sequencer) {
    _m = this;

    float sig_min = 0.0f;
    float sig_max = 1.0f;

    int sig_record_min = 0;
    int sig_record_max = 1;

    int pitch_min = 0;
    int pitch_max = 16384;

    int cc_min = 0;
    int cc_max = 127;

    float tempo_min = 20.0f;
    float tempo_max = 666.0f;

    int num_inst = 1;

    mpr_sig sig_pitch = mpr_sig_new(
        *_dev, MPR_DIR_IN, "/mpe/pitch", 1, MPR_INT32, 0, &pitch_min,
        &pitch_max, &num_inst, sig_pitch_handler, MPR_SIG_UPDATE);
    mpr_sig_new(*_dev, MPR_DIR_IN, "/mpe/pressure", 1, MPR_INT32, 0, &cc_min,
                &cc_max, &num_inst, sig_pressure_handler, MPR_SIG_UPDATE);
    mpr_sig_new(*_dev, MPR_DIR_IN, "/mpe/timbre", 1, MPR_INT32, 0, &cc_min,
                &cc_max, &num_inst, sig_timbre_handler, MPR_SIG_UPDATE);

    // mpr_sig_new(*_dev, MPR_DIR_IN, "/mod/pitch", 1, MPR_FLT, 0, &sig_min,
    //             &sig_max, &num_inst, sig_mod_pitch_handler, MPR_SIG_UPDATE);
    // mpr_sig_new(*_dev, MPR_DIR_IN, "/mod/pressure", 1, MPR_FLT, 0, &sig_min,
    //             &sig_max, &num_inst, sig_mod_pressure_handler,
    //             MPR_SIG_UPDATE);
    // mpr_sig_new(*_dev, MPR_DIR_IN, "/mod/timbre", 1, MPR_FLT, 0, &sig_min,
    //             &sig_max, &num_inst, sig_mod_timbre_handler, MPR_SIG_UPDATE);

    // mpr_sig_new(*_dev, MPR_DIR_IN, "/control/modulation", 1, MPR_FLT, 0,
    // &sig_min,
    //             &sig_max, &num_inst, sig_mod_handler, MPR_SIG_UPDATE);

    mpr_sig sig_record = mpr_sig_new(
        *_dev, MPR_DIR_IN, "/control/record", 1, MPR_INT32, 0, &sig_record_min,
        &sig_record_max, &num_inst, sig_record_handler, MPR_SIG_UPDATE);

    // mpr_sig_new(*_dev, MPR_DIR_IN, "/control/tempo", 1, MPR_FLT, 0,
    // &tempo_min,
    //             &tempo_max, &num_inst, sig_tempo_handler, MPR_SIG_UPDATE);

    while (!mpr_dev_get_is_ready(*dev)) {
      mpr_dev_poll(*dev, 25);
    }

    mpr_list sigs = mpr_dev_get_sigs(*dev, MPR_DIR_OUT);
    while (sigs) {
      mpr_sig sig = (mpr_sig)*sigs;
      const char* name = mpr_obj_get_prop_as_str(sig, MPR_PROP_NAME, NULL);
      if (strcmp(name, "raw/accl/X") == 0) {
        mpr_obj_push(mpr_map_new(1, &sig, 1, &sig_pitch));
      }
      if (strcmp(name, "button/short") == 0) {
        mpr_obj_push(mpr_map_new(1, &sig, 1, &sig_record));
      }
      sigs = mpr_list_get_next(sigs);
    }
  }

  void update() { mpr_dev_poll(*_dev, 0); }

 private:
  mpr_dev* _dev;

  GestureRecorder* _recorder;

  Sequencer* _sequencer;

  inline static Mapper* _m;

  static void sig_pitch_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                int length, mpr_type type, const void* value,
                                mpr_time time) {
    _m->_recorder->set_pitch(*static_cast<const int*>(value));
  }

  static void sig_pressure_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                   int length, mpr_type type, const void* value,
                                   mpr_time time) {
    _m->_recorder->set_pressure(*static_cast<const int*>(value));
  }

  static void sig_timbre_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                 int length, mpr_type type, const void* value,
                                 mpr_time time) {
    _m->_recorder->set_timbre(*static_cast<const int*>(value));
  }

  static void sig_mod_pitch_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                    int length, mpr_type type,
                                    const void* value, mpr_time time) {
    _m->_recorder->set_pitch_mod(*static_cast<const float*>(value));
  }

  static void sig_mod_pressure_handler(mpr_sig sig, mpr_sig_evt evt,
                                       mpr_id inst, int length, mpr_type type,
                                       const void* value, mpr_time time) {
    _m->_recorder->set_pressure_mod(*static_cast<const float*>(value));
  }

  static void sig_mod_timbre_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                     int length, mpr_type type,
                                     const void* value, mpr_time time) {
    _m->_recorder->set_timbre_mod(*static_cast<const float*>(value));
  }

  static void sig_mod_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                              int length, mpr_type type, const void* value,
                              mpr_time time) {
    _m->_recorder->set_modulation(*static_cast<const float*>(value));
  }

  static void sig_record_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                 int length, mpr_type type, const void* value,
                                 mpr_time time) {
    int val = *static_cast<const int*>(value);
    _m->_recorder->set_is_recording(val);
  }

  static void sig_tempo_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                int length, mpr_type type, const void* value,
                                mpr_time time) {
    _m->_sequencer->set_tempo(*static_cast<const float*>(value));
  }
};
}  // namespace MapLooper
