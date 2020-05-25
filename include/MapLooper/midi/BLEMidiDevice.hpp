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
#include <cstring>

#include "blemidi.h"
#include "esp_attr.h"
#include "esp_log.h"

namespace MapLooper {
class BLEMidiDevice {
 public:
  BLEMidiDevice() {
    blemidi_init(reinterpret_cast<void*>(_callback_ble_midi_message_received));
  }

  void note_on(uint8_t pitch, uint8_t velocity, uint8_t channel) {
    uint8_t data[3] = {static_cast<uint8_t>(0x90 | channel), pitch, velocity};
    blemidi_send_message(0, data, 3);
  }

  void note_off(uint8_t pitch, uint8_t channel) {
    uint8_t data[3] = {static_cast<uint8_t>(0x90 | channel), pitch, 0};
    blemidi_send_message(0, data, 3);
  }

  void cc(int num, int val, int channel);
  void all_sound_off(int channel);
  void prg_change(int prg, int channel);
  void beat_clock();
  void active_sensing() {
    uint8_t data[1] = {0xfe};
    blemidi_send_message(0, data, 1);
  }
  void start() {
    uint8_t data[1] = {0xfa};
    blemidi_send_message(0, data, 1);
  }
  void stop() {
    uint8_t data[1] = {0xfc};
    blemidi_send_message(0, data, 1);
  }
  void song_position_reset();
  void mpe_config();
  void pitch_bend(int val, int channel);
  void pressure(int val, int channel);
  void timbre(int val, int channel);

  void flush() { blemidi_tick(); }

 private:
  static void _callback_ble_midi_message_received(
      uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status,
      uint8_t* remaining_message, size_t len, size_t continued_sysex_pos) {
    ESP_LOGI(TAG,
             "CALLBACK blemidi_port=%d, timestamp=%d, midi_status=0x%02x, "
             "len=%d, continued_sysex_pos=%d, remaining_message:",
             blemidi_port, timestamp, midi_status, len, continued_sysex_pos);
    esp_log_buffer_hex(TAG, remaining_message, len);
  }

  static constexpr char TAG[] = "BLEMidiDevice";
};
}  // namespace MapLooper
