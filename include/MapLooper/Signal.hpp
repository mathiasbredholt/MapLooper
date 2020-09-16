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

#include <unordered_map>

#include "esp_log.h"
#include "mpr/mpr.h"

namespace MapLooper {
class Sequencer;
class Signal {
 public:
  typedef void (*Callback)(int trackId, const std::string& path, float value);
  typedef void (*InputChangedCallback)(Signal*);
  typedef std::unordered_map<std::string, Signal> Map;

  Signal(std::string path, float minValue, float maxValue, Callback callback,
         const InputChangedCallback& inputChangedCb, mpr_dev dev,
         Sequencer* sequencer)
      : _path(path),
        _minValue(minValue),
        _maxValue(maxValue),
        _callback(callback),
        _inputChangedCb(inputChangedCb),
        parentSequencer(sequencer) {
    esp_log_level_set(_getTag(), ESP_LOG_WARN);
    int numInst = 1;
    _inputSignal = mpr_sig_new(dev, MPR_DIR_IN, path.c_str(), 1, MPR_FLT, 0,
                               &_minValue, &_maxValue, &numInst,
                               _inputSignalUpdateHandler, MPR_SIG_UPDATE);
    mpr_obj_set_prop((mpr_obj)_inputSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR,
                     this, 0);
    std::string outputSignalName = "output/" + std::string(path);
    _outputSignal =
        mpr_sig_new(dev, MPR_DIR_OUT, outputSignalName.c_str(), 1, MPR_FLT, 0,
                    &_minValue, &_maxValue, 0, 0, MPR_SIG_UPDATE);
    ESP_LOGI(_getTag(), "Created signal '%s'", _path.c_str());
    ESP_LOGI(_getTag(), "Signal ptr %p", this);
    // ESP_LOGI(_getTag(), "callback %p", _callback);
    ESP_LOGI(_getTag(), "inputChangedCb %p", _inputChangedCb);
  }

  void updatePtr() {
    mpr_obj_set_prop((mpr_obj)_inputSignal, MPR_PROP_DATA, NULL, 1, MPR_PTR,
                     this, 0);
    ESP_LOGI(_getTag(), "Updated ptr %p", this);
  }

  inline const Callback& getCallback() const { return _callback; }

  inline float getRange() const { return _maxValue - _minValue; }

  inline float getMin() const { return _minValue; }

  inline float getMax() const { return _maxValue; }

  inline const std::string& getPath() const { return _path; }

  inline float getValue() const { return _value; }

  void update(float value) {
    mpr_sig_set_value(_outputSignal, 0, 1, MPR_FLT, &value, MPR_NOW);
  }

  Sequencer* getSequencer() { return parentSequencer; }

 private:
  static const char* _getTag() { return "Signal"; };

  static void _inputSignalUpdateHandler(mpr_sig sig, mpr_sig_evt evt,
                                        mpr_id inst, int length, mpr_type type,
                                        const void* value, mpr_time time) {
    Signal* signal =
        (Signal*)mpr_obj_get_prop_as_ptr((mpr_obj)sig, MPR_PROP_DATA, 0);
    signal->_value = *static_cast<const float*>(value);
    ESP_LOGI(_getTag(), "New value: %f", signal->_value);
    ESP_LOGI(_getTag(), "ptr %p", signal);
    ESP_LOGI(_getTag(), "minValue %f", signal->_minValue);
    ESP_LOGI(_getTag(), "maxValue %f", signal->_maxValue);
    ESP_LOGI(_getTag(), "inputChangedCb %p", signal->_inputChangedCb);
    signal->_inputChangedCb(signal);
  }

  std::string _path;
  float _minValue, _maxValue;
  Callback _callback;
  InputChangedCallback _inputChangedCb;
  Sequencer* parentSequencer;
  mpr_sig _outputSignal;
  mpr_sig _inputSignal;
  float _value;
};

}  // namespace MapLooper
