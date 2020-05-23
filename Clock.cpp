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
