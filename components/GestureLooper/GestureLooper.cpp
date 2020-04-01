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

GestureLooper::GestureLooper(mapper_device* libmapper_device)
    : sequencer(&_ptn),
      mapper(libmapper_device, &_ptn, &sequencer) {
  midi::init();
}

void GestureLooper::update() {
  // mapper.update();
  sequencer.update();
}

}  // namespace GestureLooper
