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

#include "GestureLooper/Util.hpp"
#include "GestureLooper/Mapper.hpp"
#include "GestureLooper/Pattern.hpp"
#include "GestureLooper/Sequencer.hpp"

namespace GestureLooper {

class GestureLooper {
 public:
  void* operator new(size_t size) {
    // printf("Allocating %d bytes in SPIRAM..\n", size);
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  static const int PREVIEW_TIME = 400;
  static const int LED_UPDATE_TIME = 10;

  GestureLooper(mpr_dev* libmapper_device);

  void update();

  Pattern _ptn;

  Sequencer sequencer;

  Mapper mapper;

};    
}  // namespace GestureLooper
