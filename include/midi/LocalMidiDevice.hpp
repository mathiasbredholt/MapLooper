/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

LocalMidiDevice.hpp
Midi device for onboard synthesis

*/

#pragma once

#include <cstdint>
#include <functional>

namespace MapLooper {
class LocalMidiDevice {
 public:
  LocalMidiDevice() {}

  std::function<void(uint8_t pitch, uint8_t velocity, uint8_t channel)>
      note_on = [](uint8_t pitch, uint8_t velocity, uint8_t channel) {};

  std::function<void(uint8_t pitch, uint8_t channel)> note_off =
      [](uint8_t pitch, uint8_t channel) {};

  void cc(int num, int val, int channel) {}
  void all_sound_off(int channel) {}
  void prg_change(int prg, int channel) {}
  void beat_clock() {}
  void active_sensing() {}
  void start() {}
  void stop() {}
  void song_position_reset() {}
  void mpe_config() {}
  void pitch_bend(int val, int channel) {}
  void pressure(int val, int channel) {}
  void timbre(int val, int channel) {}

  void flush() {}
};
}  // namespace MapLooper
