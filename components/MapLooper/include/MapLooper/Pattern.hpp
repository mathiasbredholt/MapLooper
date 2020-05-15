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

  Pattern(MidiOut* midiOut) : _midiOut(midiOut) {
    for (int i = 0; i < NUM_TRACKS; ++i) {
      tracks[i].id = i;
      tracks[i]._midiOut = _midiOut;
    }
    playStates[0] = false;
    clear();
  }

  void update(Tick tick, const SignalInfoMap& signalInfoMap) {
    // if (isFinite() && t % getLength() == 0) {
    //   resetTo(t / getLength() * getLength());
    // }
    // Schedule tracks
    for (Track& t : tracks) {
      if (isEnabled(t)) {
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
    length.set(8);
    quantization.set(8);
    for (Track& r : tracks) {
      r.clear();
    }
    playStates[0] = true;
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

  bool isEnabled(const Track& track) { return playStates[track.id]; }

  bool isEnabled(int id) { return playStates[id]; }

  void setPlayState(int id, bool state) { playStates[id] = state; };

  int getParam(param_name_t name) {
    if (name == PATTERN_LENGTH) {
      return length.get();
    } else if (name == QUANTIZE) {
      return quantization.get();
    } else if (name < NUM_TRACK_PARAMS) {
      return getActiveTrack().getParam(name);
    } else {
      return 0;
    }
  }

  bool isFinite() { return length.get() != 8; }

  // Tick getLength() { return PTN_LENGTH_PRESETS[length.get()]; }
  Tick getLength() { return 768; }

  Tick getQuantization() { return PTN_LENGTH_PRESETS[quantization.get()]; }

  static Track* trackToCopy;

  Tick resetPoint{0};

  std::array<Track, NUM_TRACKS> tracks;

  std::bitset<NUM_TRACKS> playStates{0};

  Parameter<uint8_t> quantization;

  Parameter<uint8_t> length;

  MidiOut* _midiOut;
};
}  // namespace MapLooper
