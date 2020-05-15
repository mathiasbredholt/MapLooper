/*
           __
  /\/\    / /
 /    \  / /
/ /\/\ \/ /___
\/    \/\____/
MapLooper
(c) Mathias Bredholt 2020

*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*-0-*

Track.hpp
Class to represent Euclidean tracks with groove system

*/

#pragma once

#include "MapLooper/Config.hpp"
#include "MapLooper/EventQueue.hpp"
#include "MapLooper/Modulation.hpp"
#include "MapLooper/Parameter.hpp"
#include "MapLooper/SignalInfo.hpp"
#include "MapLooper/TrackParameters.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "esp_log.h"

namespace MapLooper {

const int MAX_EVENTS = 64;

class Pattern;

class Track {
  friend Pattern;

 private:
  inline static const char* _getTag() { return "Track"; };

  Modulation _modulation;

  MidiOut* _midiOut;

  std::array<SignalDataMap, 768> _signalDataSequence;

 public:
  struct note_t {
    int32_t time_stamp;
    int i;
    int j;
    int8_t pitch;
    uint8_t velocity;
    int32_t note_off;
    uint8_t channel;
    bool is_top_note;
  };

  uint8_t id{0};

  TrackParameters params;

  typedef std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS>
      OffEventQueueType;

  Track() { esp_log_level_set(_getTag(), ESP_LOG_WARN); }

  static int getDivisionPreset(int idx) {
    static int PRESETS[] = {384, 192, 96,  48, 24, 12, 6, 6,
                            128, 128, 128, 64, 32, 16, 8, 8};
    return PRESETS[idx];
  }

  int getParam(param_name_t name) const { return params.getParam(name); }

  int getDiv() const { return getDivisionPreset(getParam(DIVISION)); }

  bool setParamRelative(param_name_t name, int val) {
    return params.setParamRelative(name, val);
  }

  void set_param(param_name_t name, int val, bool clear) {
    params.params[name].set(val);
  }

  void record(Tick tick, const SignalDataMap& values) {
    _signalDataSequence[tick % 768] = values;
  }

  void update(Tick tick, Tick patternLength,
              const SignalInfoMap& signalInfoMap) {
    _modulation.update(tick, patternLength);

    tick %= 768;

    for (const auto& d : _signalDataSequence[tick]) {
      // ESP_LOGI(_getTag(), "'%s': %f", d.first.c_str(), d.second);
      const SignalInfo& signalInfo = signalInfoMap.at(d.first);
      signalInfo.getCallback()(
          id, d.first, _modulation.getModulation(d.second, tick, signalInfo));
    }

    // Handle note off events
    // while (getOffEventQueue()[id].size() &&
    //        tick >= getOffEventQueue()[id].top().time_stamp) {
    //   off_event_t e = getOffEventQueue()[id].top();
    //   _midiOut->note_off(e.pitch, e.channel);
    //   getOffEventQueue()[id].pop();
    // }
  }

  int getMpeChannel() { return clip<int>(id + 1, 1, 15); }

  void playNote(note_t* n, Tick t) {
    _midiOut->note_on(n->pitch, n->velocity, getMpeChannel());

    off_event_t off_event;
    off_event.pitch = n->pitch;
    off_event.time_stamp = n->note_off;
    off_event.channel = getMpeChannel();
    getOffEventQueue()[id].push(off_event);
  }

  void releaseNotes() {
    while (getOffEventQueue()[id].size()) {
      off_event_t e = getOffEventQueue()[id].top();
      _midiOut->note_off(e.pitch, e.channel);
      getOffEventQueue()[id].pop();
    }
  }

  void clear() {
    releaseNotes();
    for (int i = 0; i < NUM_TRACK_PARAMS; ++i) {
      clearParam(static_cast<param_name_t>(i));
    }
  }

  void clearParam(param_name_t name) { params.clearParam(name); }

  static OffEventQueueType& getOffEventQueue() {
    static OffEventQueueType offEventQueue;
    return offEventQueue;
  }
};

}  // namespace MapLooper
