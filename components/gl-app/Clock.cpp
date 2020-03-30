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

Clock.cpp
A clock module that interfaces with Ableton Link.

*/

#include "gl-app/Clock.hpp"

#ifdef ESP_PLATFORM
// Missing implementations for asio
unsigned int if_nametoindex(const char *ifname) { return 0; }

char *if_indextoname(unsigned int ifindex, char *ifname) { return nullptr; }
// -----------------------------------
#endif

namespace gl {
Clock::Clock() : link_(DEFAULT_TEMPO) {
  link_.enableStartStopSync(true);
  link_.enable(true);
}

void Clock::reset() {
  auto state = link_.captureAudioSessionState();
  state.requestBeatAtTime(0.0, link_.clock().micros(), q);
  link_.commitAudioSessionState(state);
}

void Clock::start() {
  auto state = link_.captureAudioSessionState();
  state.setIsPlaying(true, link_.clock().micros());
  link_.commitAudioSessionState(state);
}

void Clock::stop() {
  auto state = link_.captureAudioSessionState();
  state.setIsPlaying(false, link_.clock().micros());
  link_.commitAudioSessionState(state);
}

int32_t Clock::get_ticks() const {
  auto state = link_.captureAudioSessionState();
  return std::round(state.beatAtTime(link_.clock().micros(), q) *
                    TICKS_PER_QUARTER_NOTE);
}

float Clock::get_tempo() const {
  auto state = link_.captureAudioSessionState();
  return state.tempo();
}

void Clock::set_tempo(float tempo) {
  auto state = link_.captureAudioSessionState();
  state.setTempo(tempo, link_.clock().micros());
  link_.commitAudioSessionState(state);
}

bool Clock::is_linked() const { return link_.numPeers() > 0; }

void Clock::set_start_stop_callback(std::function<void(bool)> callback) {
  link_.setStartStopCallback(callback);
}
}  // namespace gl
