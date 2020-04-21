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

GestureLooper.cpp
Top level module

*/

#include "GestureLooper/GestureLooper.hpp"

namespace GestureLooper {

GestureLooper::GestureLooper(mpr_dev* libmapper_device)
    : sequencer(&_ptn),
      mapper(libmapper_device, sequencer.get_recorder(), &sequencer) {
  midi::init();
}

void GestureLooper::update() {
  // mapper.update();
  sequencer.update();
  midi::flush();
}

}  // namespace GestureLooper
