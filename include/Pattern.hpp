/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Pattern.hpp
Class to contain a set of Tracks

*/

#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstdint>

#include "MapLooper/Clock.hpp"
#include "MapLooper/Parameter.hpp"
#include "MapLooper/SignalInfo.hpp"
#include "MapLooper/Track.hpp"

class Bank;
class PatternController;
class Torso;

namespace MapLooper {
class Pattern {
  friend class Bank;
  friend class PatternController;
  friend class Torso;

 public:
  uint8_t id{0};

  uint8_t activeTrackID{0};

  Pattern(MidiOut* midiOut) : _midiOut(midiOut) { clear(); }

  void update(Tick tick, const SignalInfoMap& signalInfoMap) {
    if (tick % getLength() == 0) {
      resetTo(tick / getLength() * getLength());
    }
    // Schedule tracks
    for (Track& t : tracks) {
      if (t.getEnabled()) {
        t.update(tick - resetPoint, getLength(), signalInfoMap);
      }
    }
  }

  void resetTo(int32_t time_point) {
    resetPoint = time_point;
    for (Track& r : tracks) {
      r.releaseNotes();
    }
  }

  void clear() {
    length = 384;
    for (Track& r : tracks) {
      r.clear();
    }
    activeTrackID = 0;
  }

  void releaseAll() {
    for (Track& r : tracks) {
      r.releaseNotes();
    }
  }

  Track& getActiveTrack() { return tracks[activeTrackID]; }

  void setActiveTrack(int id) { activeTrackID = id; }

  int32_t getLocalTicks(int32_t ticks) const { return ticks - resetPoint; }

  Tick getLength() { return length; }

  static Track* trackToCopy;

  Tick resetPoint{0};

  std::vector<Track> tracks;

  int length = {384};

  MidiOut* _midiOut;
};
}  // namespace MapLooper
