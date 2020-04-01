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

#include "GestureLooper/Mapper.hpp"

namespace GestureLooper {
Mapper::Mapper(mapper_device* dev, Pattern* ptn, Sequencer* sequencer)
    : dev_(dev), ptn_(ptn), sequencer_(sequencer) {

  float sig_min = 0.0f;
  float sig_max = 1.0f;

  int sig_record_min = 0;
  int sig_record_max = 1;

  float pitch_min = -24.0f;
  float pitch_max = 24.0f;

  float tempo_min = 20.0f;
  float tempo_max = 255.0f;

  mapper_device_add_input_signal(*dev_, "/mpe/pitch", 1, 'f', 0, &pitch_min,
                                 &pitch_max, sig_pitch_handler, this);
  mapper_device_add_input_signal(*dev_, "/mpe/pressure", 1, 'f', 0, &sig_min,
                                 &sig_max, sig_pressure_handler, this);
  mapper_device_add_input_signal(*dev_, "/mpe/timbre", 1, 'f', 0, &sig_min,
                                 &sig_max, sig_timbre_handler, this);

  mapper_device_add_input_signal(*dev_, "/mod/pitch", 1, 'f', 0, &sig_min,
                                 &sig_max, sig_mod_pitch_handler, this);
  mapper_device_add_input_signal(*dev_, "/mod/pressure", 1, 'f', 0, &sig_min,
                                 &sig_max, sig_mod_pressure_handler, this);
  mapper_device_add_input_signal(*dev_, "/mod/timbre", 1, 'f', 0, &sig_min,
                                 &sig_max, sig_mod_timbre_handler, this);

  mapper_device_add_input_signal(*dev_, "/control/modulation", 1, 'f', 0,
                                 &sig_min, &sig_max, sig_mod_handler, this);

  mapper_device_add_input_signal(*dev_, "/control/record", 1, 'i', 0,
                                 &sig_record_min, &sig_record_max,
                                 sig_record_handler, this);

  mapper_device_add_input_signal(*dev_, "/control/tempo", 1, 'f', 0, &tempo_min,
                                 &tempo_max, sig_tempo_handler, this);
}

void Mapper::update() {
  int count = 10;
  while (count-- && mapper_device_poll(*dev_, 0)) {
  }
}

void Mapper::sig_pitch_handler(mapper_signal sig, mapper_id instance,
                               const void* value, int count,
                               mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()->gesture_recorder.set_pitch(
      *static_cast<const float*>(value) / 24.0f);
}

void Mapper::sig_pressure_handler(mapper_signal sig, mapper_id instance,
                                  const void* value, int count,
                                  mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()->gesture_recorder.set_pressure(
      *static_cast<const float*>(value));
}

void Mapper::sig_timbre_handler(mapper_signal sig, mapper_id instance,
                                const void* value, int count,
                                mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()->gesture_recorder.set_timbre(
      *static_cast<const float*>(value));
}

void Mapper::sig_mod_pitch_handler(mapper_signal sig, mapper_id instance,
                                   const void* value, int count,
                                   mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()
      ->gesture_recorder.set_pitch_mod(*static_cast<const float*>(value));
}

void Mapper::sig_mod_pressure_handler(mapper_signal sig, mapper_id instance,
                                      const void* value, int count,
                                      mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()
      ->gesture_recorder.set_pressure_mod(*static_cast<const float*>(value));
}

void Mapper::sig_mod_timbre_handler(mapper_signal sig, mapper_id instance,
                                    const void* value, int count,
                                    mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()
      ->gesture_recorder.set_timbre_mod(*static_cast<const float*>(value));
}

void Mapper::sig_mod_handler(mapper_signal sig, mapper_id instance,
                             const void* value, int count,
                             mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->ptn_->active_track()
      ->gesture_recorder.set_modulation(*static_cast<const float*>(value));
}

void Mapper::sig_record_handler(mapper_signal sig, mapper_id instance,
                                const void* value, int count,
                                mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  int val = *static_cast<const int*>(value);
  m->ptn_->active_track()
      ->gesture_recorder.set_is_recording(val);
}

void Mapper::sig_tempo_handler(mapper_signal sig, mapper_id instance,
                               const void* value, int count,
                               mapper_timetag_t* tt) {
  Mapper* m = reinterpret_cast<Mapper*>(mapper_signal_user_data(sig));
  m->sequencer_->set_tempo(*static_cast<const float*>(value));
}
}  // namespace GestureLooper
