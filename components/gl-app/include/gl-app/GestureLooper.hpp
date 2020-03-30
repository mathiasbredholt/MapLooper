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

GestureLooper.hpp
Top level module

*/

#pragma once

#include <iostream>

#include "esp_heap_caps.h"

#include "gl-app/Mapper.hpp"
#include "gl-app/Pattern.hpp"
#include "gl-app/Sequencer.hpp"

namespace gl {

class GestureLooper {
 public:
  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  GestureLooper();

  void main_task();

  Pattern _ptn;

  Sequencer sequencer;

  Mapper mapper;

  void* operator new(size_t size) {
    printf("Allocating %d bytes in SPIRAM..\n", size);
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }
};    
}  // namespace gl
