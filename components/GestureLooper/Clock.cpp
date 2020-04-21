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

#include "GestureLooper/Clock.hpp"

#include "ableton/Link.hpp"

#ifdef ESP_PLATFORM
// Missing implementations for asio
unsigned int if_nametoindex(const char *ifname) { return 0; }

char *if_indextoname(unsigned int ifindex, char *ifname) { return nullptr; }
// -----------------------------------
#endif

namespace GestureLooper {
Clock::Clock() : _link(new ableton::Link(DEFAULT_TEMPO)) {
  reinterpret_cast<ableton::Link *>(_link)->enableStartStopSync(true);
  reinterpret_cast<ableton::Link*>(_link)->enable(true);
  xTaskCreatePinnedToCore(
      [](void *user_data) {
        while (true) {
          ableton::link::platform::IoContext::poll();
          portYIELD();
        }
      },
      "Link", 8192, nullptr, 1, &_link_task_handle, 0);
}

void Clock::reset() {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  state.requestBeatAtTime(
      0.0, reinterpret_cast<ableton::Link *>(_link)->clock().micros(), q);
  reinterpret_cast<ableton::Link *>(_link)->commitAudioSessionState(state);
}

void Clock::start() {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  state.setIsPlaying(
      true, reinterpret_cast<ableton::Link *>(_link)->clock().micros());
  reinterpret_cast<ableton::Link *>(_link)->commitAudioSessionState(state);
}

void Clock::stop() {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  state.setIsPlaying(
      false, reinterpret_cast<ableton::Link *>(_link)->clock().micros());
  reinterpret_cast<ableton::Link *>(_link)->commitAudioSessionState(state);
}

IRAM_ATTR int32_t Clock::get_ticks() const {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  return state.beatAtTime(
             reinterpret_cast<ableton::Link *>(_link)->clock().micros(), q) *
         TICKS_PER_QUARTER_NOTE;
}

float Clock::get_tempo() const {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  return state.tempo();
}

void Clock::set_tempo(float tempo) {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  state.setTempo(tempo,
                 reinterpret_cast<ableton::Link *>(_link)->clock().micros());
  reinterpret_cast<ableton::Link *>(_link)->commitAudioSessionState(state);
}

bool Clock::is_linked() const {
  return reinterpret_cast<ableton::Link *>(_link)->numPeers() > 0;
}

void Clock::set_start_stop_callback(std::function<void(bool)> callback) {
  reinterpret_cast<ableton::Link *>(_link)->setStartStopCallback(callback);
}
}  // namespace GestureLooper
