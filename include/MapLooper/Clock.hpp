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

#include <climits>
#include <cstdint>
#include <functional>

#include "ableton/Link.hpp"

#ifdef ESP_PLATFORM
inline unsigned int if_nametoindex(const char *ifname) { return 0; }
inline char *if_indextoname(unsigned int ifindex, char *ifname) {
  return nullptr;
}
#endif

namespace MapLooper {
class Clock {
 public:
  Clock() {
    link.enableStartStopSync(true);
    link.enable(true);
  }

  void reset() {
    auto state = link.captureAudioSessionState();
    state.requestBeatAtTime(0.0, link.clock().micros(), q);
    link.commitAudioSessionState(state);
  }

  void start() {
    auto state = link.captureAudioSessionState();
    state.setIsPlaying(true, link.clock().micros());
    link.commitAudioSessionState(state);
  }

  void stop() {
    auto state = link.captureAudioSessionState();
    state.setIsPlaying(false, link.clock().micros());
    link.commitAudioSessionState(state);
  }

  int getTicks() const {
    auto state = link.captureAudioSessionState();
    return state.beatAtTime(link.clock().micros(), q) * TICKS_PER_QUARTER_NOTE;
  }

  float getTempo() const {
    auto state = link.captureAudioSessionState();
    return state.tempo();
  }

  void setTempo(float tempo) {
    auto state = link.captureAudioSessionState();
    state.setTempo(tempo, link.clock().micros());
    link.commitAudioSessionState(state);
  }

  bool isLinked() const { return link.numPeers() > 0; }

  void setStartStopCallback(std::function<void(bool)> callback) {
    link.setStartStopCallback(callback);
  }

  int getTickInterval() const {
    return 60 / (getTempo() * TICKS_PER_QUARTER_NOTE);
  }

 private:
  static constexpr int TICKS_PER_QUARTER_NOTE = 48;
  static constexpr float DEFAULT_TEMPO = 120.0f;
  static const int q = 4;

  ableton::Link link{DEFAULT_TEMPO};
};
}  // namespace MapLooper
