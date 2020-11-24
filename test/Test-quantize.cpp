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

#include <chrono>
#include <fstream>
#include <iostream>

#include "MapLooper/MapLooper.hpp"

int main(int argc, char const* argv[]) {
  std::ofstream inputFile("inputSignal.txt", std::ios::binary);
  std::ofstream outputFile("outputSignal.txt", std::ios::binary);
  std::ofstream noiseFile("noiseSignal.txt", std::ios::binary);
  std::ofstream recordFile("recordSignal.txt", std::ios::binary);
  std::ofstream beatsFile("beats.txt", std::ios::binary);

  MapLooper::MapLooper mapLooper;
  MapLooper::Loop* testLoop = mapLooper.createLoop("test");
  float ppqn = 2.0f;
  mpr_sig_set_value(testLoop->getPpqnSignal(), 0, 1, MPR_FLT, &ppqn);

  mpr_sig inputSignal = testLoop->getInputSignal();
  mpr_sig outputSignal = testLoop->getOutputSignal();
  mpr_sig noiseSignal = testLoop->getModulationSignal();
  mpr_sig recordSignal = testLoop->getRecordSignal();

  float length = 2.0f;
  mpr_sig_set_value(testLoop->getLengthSignal(), 0, 1, MPR_FLT, &length);

  float rec = 1.0f;
  float noise = 0.0f;

  mpr_sig_set_value(recordSignal, 0, 1, MPR_FLT, &rec);

  double startTime = mapLooper.getBeats();
  bool finish = false;
  int i = 0;
  for (;;) {
    // std::cout << i << " : ";
    float now = mapLooper.getBeats() - startTime - 8;
    if (now < 1.0) {
      mpr_sig_set_value(inputSignal, 0, 1, MPR_FLT, &now);
    } else if (now > 1.0 && now < 2.0) {
      float val = 0.0f;
      mpr_sig_set_value(inputSignal, 0, 1, MPR_FLT, &val);
    } else if (now > 2.0 && now < 4.0) {
      float rec = 0.0f;
      mpr_sig_set_value(recordSignal, 0, 1, MPR_FLT, &rec);
    } else if (now > 4.0 && now < 8.0) {
      noise = 0.1f;
      mpr_sig_set_value(noiseSignal, 0, 1, MPR_FLT, &noise);
    } else if (now > 8.0 && now < 12.0) {
      noise = 0.0f;
      mpr_sig_set_value(noiseSignal, 0, 1, MPR_FLT, &noise);
      // } else if (now > 12.0 && now < 16.0) {
      // rec = 0.5f;
      // mpr_sig_set_value(recordSignal, 0, 1, MPR_FLT, &rec);
    } else if (now > 12.0) {
      finish = true;
    }

    if (mpr_sig_get_value(outputSignal, 0, 0)) {
      inputFile << *((float*)mpr_sig_get_value(inputSignal, 0, 0)) << '\n';
      outputFile << *((float*)mpr_sig_get_value(outputSignal, 0, 0)) << '\n';
      noiseFile << *((float*)mpr_sig_get_value(noiseSignal, 0, 0)) << '\n';
      recordFile << *((float*)mpr_sig_get_value(recordSignal, 0, 0)) << '\n';
      beatsFile << now << '\n';
    }

    mapLooper.update(5);
    ++i;

    if (finish) {
      break;
    }
  }
  return 0;
}
