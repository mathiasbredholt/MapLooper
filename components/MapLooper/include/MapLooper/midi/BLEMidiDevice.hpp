/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

BLEMidiDevice.hpp
BLEMidiDevice class

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

  void note_on(uint8_t pitch, uint8_t velocity,
               uint8_t channel) {
    uint8_t data[3] = {static_cast<uint8_t>(0x90 | channel), pitch,
                             velocity};
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
