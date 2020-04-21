/*

          ___
         (   )
  .--.    | |
 /    \   | |
;  ,-. '  | |
| |  | |  | |
| |  | |  | |
| |  | |  | |
| '  | |  | |
'  `-' |  | |
 `.__. | (___)
 ( `-' ;
  `.__.

Gesture Looper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Mapper.hpp
libmapper interface

*/

#pragma once

#include "GestureLooper/GestureRecorder.hpp"
#include "GestureLooper/Sequencer.hpp"
#include "GestureLooper/Util.hpp"
#include "mpr/mpr.h"

namespace GestureLooper {
class Mapper {
 public:
  Mapper(mpr_dev* dev, GestureRecorder* recorder, Sequencer* sequencer);

  void update();

 private:
  mpr_dev* dev_;

  GestureRecorder* _recorder;

  Sequencer* sequencer_;

  static void sig_pitch_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                int length, mpr_type type, const void* value,
                                mpr_time time);

  static void sig_pressure_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                   int length, mpr_type type, const void* value,
                                   mpr_time time);

  static void sig_timbre_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                 int length, mpr_type type, const void* value,
                                 mpr_time time);

  static void sig_mod_pitch_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                    int length, mpr_type type,
                                    const void* value, mpr_time time);

  static void sig_mod_pressure_handler(mpr_sig sig, mpr_sig_evt evt,
                                       mpr_id inst, int length, mpr_type type,
                                       const void* value, mpr_time time);

  static void sig_mod_timbre_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                     int length, mpr_type type,
                                     const void* value, mpr_time time);

  static void sig_mod_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                              int length, mpr_type type, const void* value,
                              mpr_time time);

  static void sig_record_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                 int length, mpr_type type, const void* value,
                                 mpr_time time);

  static void sig_tempo_handler(mpr_sig sig, mpr_sig_evt evt, mpr_id inst,
                                int length, mpr_type type, const void* value,
                                mpr_time time);
};
}  // namespace GestureLooper
