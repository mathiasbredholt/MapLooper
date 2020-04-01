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

WiFi.hpp
Library for interfacing with WiFi on ESP32

*/

#pragma once

#if defined __has_include
#  if __has_include ("esp_idf_version.h")
#    include "esp_idf_version.h"
#    if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
#      define ESP_IDF_V4 
#    endif
#  endif
#endif

#ifdef ESP_IDF_V4

#include <cstring>

#include "esp_event.h"
#include "esp_log.h"
// #include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

namespace GestureLooper {
namespace wifi {

void init();

void wait_for_connection();

void start_smartconfig();

bool is_connected();

void connect_to_next();

int num_saved_networks();

esp_err_t save_wifi_network(wifi_config_t* wifi_config);

}  // namespace wifi
}  // namespace GestureLooper

#endif
