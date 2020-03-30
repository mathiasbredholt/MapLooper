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

#ifdef ESP_PLATFORM
// Missing implementations for asio
unsigned int if_nametoindex(const char *ifname) { return 0; }

char *if_indextoname(unsigned int ifindex, char *ifname) { return nullptr; }
// -----------------------------------
#endif

namespace GestureLooper {
Clock::Clock() : _link(DEFAULT_TEMPO) {
  _link.enableStartStopSync(true);
  _link.enable(true);
  xTaskCreatePinnedToCore(
      [](void *user_data) {
        while (true) {
          ableton::link::platform::IoContext::poll();
          vTaskDelay(1);
        }
      },
      "Link", 8192, nullptr, 1, &_link_task_handle, 0);
}

void Clock::reset() {
  auto state = _link.captureAudioSessionState();
  state.requestBeatAtTime(0.0, _link.clock().micros(), q);
  _link.commitAudioSessionState(state);
}

void Clock::start() {
  auto state = _link.captureAudioSessionState();
  state.setIsPlaying(true, _link.clock().micros());
  _link.commitAudioSessionState(state);
}

void Clock::stop() {
  auto state = _link.captureAudioSessionState();
  state.setIsPlaying(false, _link.clock().micros());
  _link.commitAudioSessionState(state);
}

int32_t Clock::get_ticks() const {
  auto state = _link.captureAudioSessionState();
  return std::round(state.beatAtTime(_link.clock().micros(), q) *
                    TICKS_PER_QUARTER_NOTE);
}

float Clock::get_tempo() const {
  auto state = _link.captureAudioSessionState();
  return state.tempo();
}

void Clock::set_tempo(float tempo) {
  auto state = _link.captureAudioSessionState();
  state.setTempo(tempo, _link.clock().micros());
  _link.commitAudioSessionState(state);
}

bool Clock::is_linked() const { return _link.numPeers() > 0; }

void Clock::set_start_stop_callback(std::function<void(bool)> callback) {
  _link.setStartStopCallback(callback);
}
}  // namespace GestureLooper
