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

#include "gl-app/GestureLooper.hpp"

namespace gl {

GestureLooper::GestureLooper() : sequencer(nullptr), mapper(&_ptn, &sequencer) {
  midi::init();
}

void GestureLooper::main_task() {
  mapper.update();
  sequencer.update();
}

}  // namespace gl
