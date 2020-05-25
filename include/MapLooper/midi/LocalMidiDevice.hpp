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
