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

MIDI.hpp
MIDI

*/

#pragma once

#include <cstring>

#ifdef ESP_PLATFORM
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "blemidi.h"
#else
#include "RtMidi.h"
#endif

namespace gl {

namespace midi {
const int MAX_DATA_SIZE = 12;

struct message_t {
  uint8_t data[MAX_DATA_SIZE];
  uint8_t size;
};

// 2 DATA BYTES
const int NOTE_ON = 0x90;
const int CC = 0xb0;
const int SONG_POS = 0xf2;

// 1 DATA BYTE
const int PRG_CHANGE = 0xc0;

// REAL TIME
const int BEAT_CLOCK = 0xf8;
const int START_TRANSPORT = 0xfa;
const int STOP_TRANSPORT = 0xfc;
const int ACTIVE_SENSING = 0xfe;

void note_on(message_t* msg, int pitch, int velocity, int channel);
void note_off(message_t* msg, int pitch, int channel);
void cc(message_t* msg, int num, int val, int channel);
void all_sound_off(message_t* msg, int channel);
void prg_change(message_t* msg, int prg, int channel);
void beat_clock(message_t* msg);
void active_sensing(message_t* msg);
void start(message_t* msg);
void stop(message_t* msg);
void song_position_reset(message_t* msg);
void mpe_config(message_t *msg);
void pitch_bend(message_t *msg, int val, int channel);
void pressure(message_t *msg, int val, int channel);
void timbre(message_t *msg, int val, int channel);

enum midi_port_t { ALL, DIN, USB };

void init();

void send(message_t* msg, midi_port_t port);

void flush();

#ifdef I2C_MIDI
void i2c_init();
#endif
}  // namespace midi

}  // namespace gl
