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

MIDI.cpp
MIDI
*/

#include "GestureLooper/MIDI.hpp"

namespace GestureLooper {
namespace midi {
#ifdef ESP_PLATFORM
const char TAG[] = "MIDI_IO";
// template <int BUF_SIZE>
// struct midi_buffer_t {
//   uint8_t data[BUF_SIZE] = {0};
//   int size = 0;
// };

// class BleMidiBuffer {
//  public:
//   static const int BUFFER_SIZE = 256;
//   void write(uint8_t *data, int n) {
//     if (buf.size + n > BUFFER_SIZE) return;
//     std::memcpy(buf.data + buf.size, data, n);
//     buf.size += n;
//   }

//   void flush();
//   midi_buffer_t<BUFFER_SIZE> buf;
// };


void callback_ble_midi_message_received(uint8_t blemidi_port,
                                        uint16_t timestamp, uint8_t midi_status,
                                        uint8_t *remaining_message, size_t len,
                                        size_t continued_sysex_pos) {
  ESP_LOGI(TAG,
           "CALLBACK blemidi_port=%d, timestamp=%d, midi_status=0x%02x, "
           "len=%d, continued_sysex_pos=%d, remaining_message:",
           blemidi_port, timestamp, midi_status, len, continued_sysex_pos);
  esp_log_buffer_hex(TAG, remaining_message, len);
}

void init() {
  int status = blemidi_init(reinterpret_cast<void*>(callback_ble_midi_message_received));
}

void send(message_t *msg) {
  blemidi_send_message(0, msg->data, msg->size);
}

void flush() {
  blemidi_tick();
}

#else
RtMidiOut *midiout;

void init() {
  midiout = new RtMidiOut();
  midiout->openVirtualPort("t|so");
}

void send(message_t *msg, midi_port_t port) {
#ifndef TEST
  midiout->sendMessage(msg->data, msg->size);
#endif
}

void flush() {}
#endif

void note_on(message_t *msg, int pitch, int velocity, int channel) {
  msg->data[0] = NOTE_ON | channel;
  msg->data[1] = pitch;
  msg->data[2] = velocity;
  msg->size = 3;
}

void note_off(message_t *msg, int pitch, int channel) {
  msg->data[0] = NOTE_ON | channel;
  msg->data[1] = pitch;
  msg->data[2] = 0;
  msg->size = 3;
}

void all_sound_off(message_t *msg, int channel) {
  msg->data[0] = CC | channel;
  msg->data[1] = 120;
  msg->data[2] = 0;
  msg->size = 3;
}

void prg_change(message_t *msg, int prg, int channel) {
  msg->data[0] = PRG_CHANGE | channel;
  msg->data[1] = prg;
  msg->size = 2;
}

void beat_clock(message_t *msg) {
  msg->data[0] = BEAT_CLOCK;
  msg->size = 1;
}

void active_sensing(message_t *msg) {
  msg->data[0] = ACTIVE_SENSING;
  msg->size = 1;
}

void start(message_t *msg) {
  msg->data[0] = START_TRANSPORT;
  msg->size = 1;
}

void stop(message_t *msg) {
  msg->data[0] = STOP_TRANSPORT;
  msg->size = 1;
}

void song_position_reset(message_t *msg) {
  msg->data[0] = SONG_POS;
  msg->data[1] = 0;
  msg->data[2] = 0;
  msg->size = 3;
}

void pitch_bend(message_t *msg, int val, int channel) {
  msg->data[0] = 0xe0 | channel;
  msg->data[1] = val & 0x1f;
  msg->data[2] = (val & 0x3f80) >> 7;
  msg->size = 3;
}

void pressure(message_t *msg, int val, int channel) {
  msg->data[0] = 0xd0 | channel;
  msg->data[1] = val;
  msg->size = 2;
}

void timbre(message_t *msg, int val, int channel) {
  msg->data[0] = 0xb0 | channel;
  msg->data[1] = 0x4a;
  msg->data[2] = val;
  msg->size = 3;
}

void mpe_config(message_t *msg) {
  static const uint8_t d[12] = {
      0xb0, 0x79, 0x00,  // Reset all controllers
      0xb0, 0x64, 0x06,  // RPN MPE LSB
      0xb0, 0x65, 0x00,  // RPN MPE MSB
      0xb0, 0x06, 0x0f   // Set Lower zone to 15 channels
  };
  std::memcpy(msg->data, d, 9);
  msg->size = 9;
}
}  // namespace midi
}  // namespace GestureLooper
