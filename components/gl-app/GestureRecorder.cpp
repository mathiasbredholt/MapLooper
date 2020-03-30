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

GestureRecorder.cpp
Class to record gestures

*/

#include "gl-app/GestureRecorder.hpp"

namespace gl {
GestureVoice::GestureVoice(GestureRecorder* gestureRecorder, int channel,
                           int duration, int32_t start_time)
    : gestureRecorder_(gestureRecorder),
      channel_(channel),
      duration_(duration),
      start_time_(start_time) {}

void GestureVoice::update(int32_t t) {
  int32_t tt = t % duration_;
  midi::message_t msg;

  float pitch_data = gestureRecorder_->data_[0][tt];
  float pressure_data = gestureRecorder_->data_[1][tt];
  float timbre_data = gestureRecorder_->data_[2][tt];

  pitch_data += gestureRecorder_->rand_data_[0][tt] *
                gestureRecorder_->rand_modulation_pitch_;
  pressure_data += gestureRecorder_->rand_data_[1][tt] *
                   gestureRecorder_->rand_modulation_pressure_;
  timbre_data += gestureRecorder_->rand_data_[2][tt] *
                 gestureRecorder_->rand_modulation_timbre_;

  int d = clip<int>(8192 + pitch_data * 16384, 0, 16383);
  if (d != last_data_[0]) {
    midi::pitch_bend(&msg, d, channel_);
    midi::send(&msg, midi::ALL);
    last_data_[0] = d;
  }

  d = clip<int>(pressure_data * 127, 0, 127);
  if (d != last_data_[1]) {
    midi::pressure(&msg, d, channel_);
    midi::send(&msg, midi::ALL);
    last_data_[1] = d;
  }

  d = clip<int>((timbre_data + 0.5) * 127, 0, 127);
  if (d != last_data_[2]) {
    midi::timbre(&msg, d, channel_);
    midi::send(&msg, midi::ALL);
    last_data_[2] = d;
  }
}

bool GestureVoice::is_done(int32_t t) {
  return ((t - start_time_) >= duration_) || ((t - start_time_) < 0);
}

std::random_device GestureRecorder::rd_;

GestureRecorder::GestureRecorder() : gen_(rd_()), dist_(-1.0f, 1.0f) {
  for (auto& arr : data_) {
    arr.fill(0);
  }
  for (auto& arr : rand_data_) {
    std::generate(arr.begin(), arr.end(), [this]() { return dist_(gen_); });
  }
}

void GestureRecorder::set_is_recording(bool enable) { is_recording_ = enable; }

void GestureRecorder::play(int channel, int32_t t, int32_t duration) {
  GestureVoice voice(this, channel, duration, t);
  voices_.push_back(voice);
}

void GestureRecorder::reset() { voices_.clear(); }

void GestureRecorder::set_pitch(float val) { pitch_val_ = val; }

void GestureRecorder::set_pressure(float val) { pressure_val_ = val; }

void GestureRecorder::set_timbre(float val) { timbre_val_ = val; }

void GestureRecorder::set_pitch_mod(float val) { rand_modulation_pitch_ = val; }

void GestureRecorder::set_pressure_mod(float val) {
  rand_modulation_pressure_ = val;
}

void GestureRecorder::set_timbre_mod(float val) {
  rand_modulation_timbre_ = val;
}

void GestureRecorder::set_modulation(float val) { rand_modulation_ = val; }

void GestureRecorder::update(int32_t t) {
  for (auto& v : voices_) {
    v.update(t);
  }
  // Delete finished voices
  voices_.erase(std::remove_if(voices_.begin(), voices_.end(),
                               [&](GestureVoice& v) { return v.is_done(t); }),
                voices_.end());
}

void GestureRecorder::record_gestures(int32_t t, int32_t duration) {
  if (is_recording_) {
    const int write_idx = t % duration;
    data_[0][write_idx] = pitch_val_;
    data_[1][write_idx] = pressure_val_;
    data_[2][write_idx] = timbre_val_;

    for (int i = 0; i < 3; ++i) {
      const float val = dist_(gen_);
      rand_data_[i][write_idx] += val * rand_modulation_;
      rand_data_[i][write_idx] = fold(rand_data_[i][write_idx], -1.0f, 1.0f);
    }
  }
}
}  // namespace gl
