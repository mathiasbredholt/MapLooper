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

WiFi.cpp
Library for interfacing with WiFi on ESP32

*/

#ifdef ESP_PLATFORM
#include "GestureLooper/WiFi.hpp"

namespace GestureLooper {
namespace wifi {
const char TAG[] = "network_manager";
const char STORAGE_NAMESPACE[] = "wifi_networks";

static EventGroupHandle_t s_wifi_event_group;

static const int CONNECTED_BIT = BIT0;

static const int ESPTOUCH_DONE_BIT = BIT1;

ip4_addr_t ip_addr;

bool connected = false;

bool smart_config_started = false;

struct wifi_database_t {
  wifi_config_t* config_arr = nullptr;
  int32_t num_networks = 0;
  int32_t selected_network = 0;
};

wifi_database_t db;

esp_err_t load_wifi_networks(wifi_database_t* db) {
  nvs_handle_t my_handle;
  esp_err_t err;

  // Open
  err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  // Read number of networks
  err = nvs_get_i32(my_handle, "num_networks", &db->num_networks);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

  ESP_LOGI(TAG, "Found %d saved networks.", db->num_networks);

  if (db->num_networks > 0) {
    // Read networks
    size_t config_arr_size = db->num_networks * sizeof(wifi_config_t);
    db->config_arr = reinterpret_cast<wifi_config_t*>(malloc(config_arr_size));

    err = nvs_get_blob(my_handle, "networks", db->config_arr, &config_arr_size);
    if (err != ESP_OK) {
      free(db->config_arr);
      return err;
    }

    ESP_LOGI(TAG, "Saved networks:");
    ESP_LOGI(TAG, "%-25s%-25s", "SSID", "Password");
    for (int i = 0; i < db->num_networks; ++i) {
      ESP_LOGI(TAG, "%-25s%-25s", db->config_arr[i].sta.ssid,
               db->config_arr[i].sta.password);
    }
  }

  // Close
  nvs_close(my_handle);
  return ESP_OK;
}

esp_err_t save_wifi_network(wifi_config_t* wifi_config) {
  nvs_handle_t my_handle;
  esp_err_t err;

  // Open
  err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) return err;

  // Read number of networks
  int32_t num_networks = 0;
  err = nvs_get_i32(my_handle, "num_networks", &num_networks);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

  // Read networks
  size_t config_arr_size = num_networks * sizeof(wifi_config_t);
  wifi_config_t* config_arr = reinterpret_cast<wifi_config_t*>(
      malloc(config_arr_size + sizeof(wifi_config_t)));
  if (num_networks > 0) {
    err = nvs_get_blob(my_handle, "networks", config_arr, &config_arr_size);
    if (err != ESP_OK) {
      free(config_arr);
      return err;
    }
  }

  // Increase number of networks
  ++num_networks;

  // Save number of networks
  err = nvs_set_i32(my_handle, "num_networks", num_networks);
  if (err != ESP_OK) {
    free(config_arr);
    return err;
  }

  config_arr_size = num_networks * sizeof(wifi_config_t);

  // Add network to array and write
  config_arr[num_networks - 1] = *wifi_config;
  err = nvs_set_blob(my_handle, "networks", config_arr, config_arr_size);
  free(config_arr);

  if (err != ESP_OK) return err;

  // Commit
  err = nvs_commit(my_handle);
  if (err != ESP_OK) return err;

  // Close
  nvs_close(my_handle);

  return ESP_OK;
}

wifi_config_t* get_next_config_from_db(wifi_database_t* db) {
  ++(db->selected_network);
  if (db->selected_network >= db->num_networks) {
    db->selected_network = 0;
  }
  return &(db->config_arr[db->selected_network]);
}

static void smartconfig_task(void* parm) {
  ESP_LOGI(TAG, "SmartConfig started.");
  EventBits_t uxBits;
  ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
  smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
  while (1) {
    uxBits = xEventGroupWaitBits(s_wifi_event_group,
                                 CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false,
                                 portMAX_DELAY);
    if (uxBits & CONNECTED_BIT) {
      ESP_LOGI(TAG, "WiFi Connected to ap");
    }
    if (uxBits & ESPTOUCH_DONE_BIT) {
      ESP_LOGI(TAG, "smartconfig over");
      esp_smartconfig_stop();
      smart_config_started = false;
      vTaskDelete(NULL);
    }
  }
}

void wait_for_connection() {
  xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT, true, false,
                      portMAX_DELAY);
}

bool is_connected() { return connected; }

void start_smartconfig() {
  if (!smart_config_started) {
    smart_config_started = true;
    esp_wifi_disconnect();
    xTaskCreatePinnedToCore(smartconfig_task, "SmartConfig task", 4096, nullptr,
                            15, nullptr, 0);
  }
}

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                   void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (!smart_config_started) {
      esp_wifi_connect();
      xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
      connected = false;
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    connected = true;
  } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
    ESP_LOGI(TAG, "SmartConfig: Scan done");
  } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
    ESP_LOGI(TAG, "SmartConfig: Found channel");
  } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
    ESP_LOGI(TAG, "SmartConfig: Got SSID and password");

    smartconfig_event_got_ssid_pswd_t* evt =
        (smartconfig_event_got_ssid_pswd_t*)event_data;
    wifi_config_t wifi_config;
    uint8_t ssid[33] = {0};
    uint8_t password[65] = {0};

    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, evt->password,
           sizeof(wifi_config.sta.password));
    wifi_config.sta.bssid_set = evt->bssid_set;
    if (wifi_config.sta.bssid_set == true) {
      memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
    }

    memcpy(ssid, evt->ssid, sizeof(evt->ssid));
    memcpy(password, evt->password, sizeof(evt->password));
    ESP_LOGI(TAG, "SSID:%s", ssid);
    ESP_LOGI(TAG, "PASSWORD:%s", password);

    esp_err_t err = save_wifi_network(&wifi_config);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%s) while saving network.", esp_err_to_name(err));
    } else {
      ESP_LOGI(TAG, "Saved network '%s'.", wifi_config.sta.ssid);
    }

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_connect());
  } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
    xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
  }
}

void connect_to_next() {
  if (!smart_config_started) {
    if (db.num_networks > 0) {
      wifi_config_t* wifi_config = get_next_config_from_db(&db);
      ESP_ERROR_CHECK(esp_wifi_disconnect());
      ESP_LOGI(TAG, "Disconnected. Trying to connect to network '%s'.",
               wifi_config->sta.ssid);
      ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config));
      ESP_ERROR_CHECK(esp_wifi_connect());
    }
  }
}

void init() {
  esp_log_level_set("wifi", ESP_LOG_WARN);
  esp_log_level_set("smartconfig", ESP_LOG_WARN);

  s_wifi_event_group = xEventGroupCreate();

  // ESP_ERROR_CHECK(esp_netif_init());
  tcpip_adapter_init();

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  // esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &event_handler, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
                                             &event_handler, NULL));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Load config
  esp_err_t err = load_wifi_networks(&db);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) while loading WiFi networks!",
             esp_err_to_name(err));
  }

  ESP_ERROR_CHECK(esp_wifi_start());

  // Disable power saving
  esp_wifi_set_ps(WIFI_PS_NONE);
}

int num_saved_networks() {
  return db.num_networks;
}
}  // namespace wifi
}  // namespace GestureLooper
#endif
