/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Clock.cpp
A clock module that interfaces with Ableton Link.

*/

#include "MapLooper/Clock.hpp"

#include "ableton/Link.hpp"

#ifdef ESP_PLATFORM
// Missing implementations for asio
unsigned int if_nametoindex(const char *ifname) { return 0; }

char *if_indextoname(unsigned int ifindex, char *ifname) { return nullptr; }
// -----------------------------------
#endif

namespace MapLooper {
Clock::Clock() : _link(new ableton::Link(DEFAULT_TEMPO)) {
  reinterpret_cast<ableton::Link *>(_link)->enableStartStopSync(true);
  reinterpret_cast<ableton::Link *>(_link)->enable(true);
  xTaskCreate(
      [](void *user_data) {
        while (true) {
          ableton::link::platform::IoContext::poll();
          portYIELD();
        }
      },
      "Link", 8192, nullptr, 1, &_link_task_handle);
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

int32_t Clock::getTicks() const {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  return state.beatAtTime(
             reinterpret_cast<ableton::Link *>(_link)->clock().micros(), q) *
         TICKS_PER_QUARTER_NOTE;
}

float Clock::getTempo() const {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  return state.tempo();
}

void Clock::setTempo(float tempo) {
  auto state =
      reinterpret_cast<ableton::Link *>(_link)->captureAudioSessionState();
  state.setTempo(tempo,
                 reinterpret_cast<ableton::Link *>(_link)->clock().micros());
  reinterpret_cast<ableton::Link *>(_link)->commitAudioSessionState(state);
}

bool Clock::isLinked() const {
  return reinterpret_cast<ableton::Link *>(_link)->numPeers() > 0;
}

void Clock::setStartStopCallback(std::function<void(bool)> callback) {
  reinterpret_cast<ableton::Link *>(_link)->setStartStopCallback(callback);
}

int Clock::getTickInterval() const {
  return 60 / (getTempo() * TICKS_PER_QUARTER_NOTE); 
}
}  // namespace MapLooper
