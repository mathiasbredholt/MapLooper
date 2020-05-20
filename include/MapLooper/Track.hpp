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

#include <unordered_map>

#include "MapLooper/Config.hpp"
#include "MapLooper/EventQueue.hpp"
#include "MapLooper/Modulation.hpp"
#include "MapLooper/SignalInfo.hpp"
#include "MapLooper/Util.hpp"
#include "MapLooper/midi/MidiConfig.hpp"
#include "esp_log.h"

namespace MapLooper {

const int MAX_EVENTS = 64;

typedef std::unordered_map<std::string, float> SignalDataMap;

std::array<SignalDataMap, 768> _signalDataSequence;

class Track {
 public:
  struct note_t {
    int32_t time_stamp;
    int8_t pitch;
    uint8_t velocity;
    int32_t note_off;
    uint8_t channel;
  };

  typedef std::array<event_queue_t<off_event_t, MAX_EVENTS>, NUM_TRACKS>
      OffEventQueueType;

  Track(int _id, MidiOut* midiOut) : _midiOut(midiOut) {
    esp_log_level_set(_getTag(), ESP_LOG_WARN);
  }

  static int getDivisionPreset(int idx) {
    static int PRESETS[] = {384, 192, 96,  48, 24, 12, 6, 6,
                            128, 128, 128, 64, 32, 16, 8, 8};
    return PRESETS[idx];
  }

  void record(Tick tick, const SignalDataMap& values) {
    _signalDataSequence[tick % 768] = values;
  }

  void update(Tick tick, Tick patternLength,
              const SignalInfoMap& signalInfoMap) {
    _modulation.update(tick, patternLength);

    tick %= 768;
    SignalDataMap signalDataMap = _signalDataSequence.at(tick);

    for (auto d : signalDataMap) {
      // ESP_LOGI(_getTag(), "'%s': %f", d.first.c_str(), d.second);
      const SignalInfo& signalInfo = signalInfoMap.at(d.first);
      signalInfo.getCallback()(
          _id, d.first, _modulation.getModulation(d.second, tick, signalInfo));
    }

    // Handle note off events
    // while (getOffEventQueue()[_id].size() &&
    //        tick >= getOffEventQueue()[_id].top().time_stamp) {
    //   off_event_t e = getOffEventQueue()[_id].top();
    //   _midiOut->note_off(e.pitch, e.channel);
    //   getOffEventQueue()[_id].pop();
    // }
  }

  int getMpeChannel() { return clip<int>(_id + 1, 1, 15); }

  void playNote(note_t* n, Tick t) {
    _midiOut->note_on(n->pitch, n->velocity, getMpeChannel());

    off_event_t off_event;
    off_event.pitch = n->pitch;
    off_event.time_stamp = n->note_off;
    off_event.channel = getMpeChannel();
    getOffEventQueue()[_id].push(off_event);
  }

  void releaseNotes() {
    while (getOffEventQueue()[_id].size()) {
      off_event_t e = getOffEventQueue()[_id].top();
      _midiOut->note_off(e.pitch, e.channel);
      getOffEventQueue()[_id].pop();
    }
  }

  void clear() { releaseNotes(); }

  static OffEventQueueType& getOffEventQueue() {
    static OffEventQueueType offEventQueue;
    return offEventQueue;
  }

  void setEnabled(bool state) { _isEnabled = state; }

  bool getEnabled() { return _isEnabled; }

 private:
  inline static const char* _getTag() { return "Track"; };

  Modulation _modulation;

  MidiOut* _midiOut;

  uint8_t _id{0};

  bool _isEnabled{true};
};

}  // namespace MapLooper
