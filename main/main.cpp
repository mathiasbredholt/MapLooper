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

main.cpp
main

*/

#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "GestureLooper/GestureLooper.hpp"
#include "GestureLooper/WiFi.hpp"

static const char* TAG = "main";
const int LINK_POLL_PRIORITY = 2;
const int WIFI_SWITCH_PRIORITY = 1;

void init_spiffs() {
  esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                .partition_label = nullptr,
                                .max_files = 5,
                                .format_if_mount_failed = true};
  esp_err_t ret = esp_vfs_spiffs_register(&conf);
  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
  }
}

void wifi_switch_task(void* user_param) {
  while (true) {
    bool wifi_status = GestureLooper::wifi::is_connected();
    static bool last_wifi_status;
    if (!wifi_status && wifi_status == last_wifi_status) {
      GestureLooper::wifi::connect_to_next();
    }
    last_wifi_status = wifi_status;
    ESP_LOGI(TAG, "HEAP: %d bytes", xPortGetFreeHeapSize());
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

void gl_task(void* user_param) {
  GestureLooper::GestureLooper* app = new GestureLooper::GestureLooper();
  while (true) {
    app->update();
    vTaskDelay(1);
  }
}

extern "C" void app_main() {
  // ESP_LOGI(TAG, "Gesture Looper",
  //          esp_ota_get_app_description()->version);
  // ESP_LOGI(TAG, "(c) Mathias Bredholt 2020");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  init_spiffs();
  GestureLooper::wifi::init();

  if (GestureLooper::wifi::num_saved_networks() == 0) {
    ESP_LOGI(TAG, "no wifif found, saving default");
    wifi_config_t wifi_config;
    uint8_t ssid[33] = "dlink-FA68";
    uint8_t password[65] = "kittykitty93";
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, password,
           sizeof(wifi_config.sta.password));
    GestureLooper::wifi::save_wifi_network(&wifi_config);
    GestureLooper::wifi::connect_to_next();
  }

  xTaskCreatePinnedToCore(gl_task, "gl", 4096, NULL, 10, NULL, 1);
  xTaskCreatePinnedToCore(wifi_switch_task, "wifi_switch", 2048, NULL,
                          WIFI_SWITCH_PRIORITY, NULL, 0);
  vTaskDelete(NULL);
}
