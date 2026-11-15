#include "wifi.hpp"
#include "esp_log.h"
#include "nvs_flash.h"
#include <cstring>

Wifi::Wifi() = default;

Wifi::~Wifi() {
  deinit();
}

esp_err_t Wifi::init(wifi_mode_t mode, uint8_t channel) {
  if (m_initialized) {
    ESP_LOGW(TAG, "Wifi already initialized");
    return true;
  }

  ESP_LOGI(TAG, "Initializing Wi-Fi Manager...");

  // 1. Initialize NVS (Safe to call even if already initialized)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // 2. Initialize TCP/IP network interface & default event loop
  esp_err_t netif_err = esp_netif_init();
  if (netif_err != ESP_OK && netif_err != ESP_ERR_INVALID_STATE) {
    ESP_ERROR_CHECK(netif_err);
  }

  esp_err_t loop_err = esp_event_loop_create_default();
  if (loop_err != ESP_OK && loop_err != ESP_ERR_INVALID_STATE) {
    ESP_ERROR_CHECK(loop_err);
  }

  // Create default netif instances depending on mode
  if (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) {
    esp_netif_create_default_wifi_sta();
  }
  if (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA) {
    esp_netif_create_default_wifi_ap();
  }

  // 3. Register Event Handler for future STA/AP events
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::wifi_event_handler_proxy, this, nullptr));

  // 4. Initialize and Start Wi-Fi Driver
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
  ESP_ERROR_CHECK(esp_wifi_start());

  // 5. Lock Wi-Fi Channel AFTER esp_wifi_start()
  if (channel > 0) {
    set_channel(channel, WIFI_SECOND_CHAN_NONE);
  }

  m_initialized = true;
  ESP_LOGI(TAG, "Wi-Fi initialized on channel %d", channel);

  return ESP_OK;
}

void Wifi::deinit() {
  if (!m_initialized) return;

  esp_wifi_stop();
  esp_wifi_deinit();
  m_initialized = false;
  ESP_LOGI(TAG, "Wi-Fi deinitialized");
}

bool Wifi::set_channel(uint8_t primary_channel, wifi_second_chan_t second) {
  esp_err_t err = esp_wifi_set_channel(primary_channel, second);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set Wi-Fi channel %d: %s", primary_channel, esp_err_to_name(err));
    return false;
  }
  return true;
}

uint8_t Wifi::get_channel() const {
  uint8_t primary = 0;
  wifi_second_chan_t second;
  esp_wifi_get_channel(&primary, &second);
  return primary;
}

bool Wifi::get_mac_address(uint8_t *mac_out, wifi_interface_t iface) const {
  return (esp_wifi_get_mac(iface, mac_out) == ESP_OK);
}

bool Wifi::connect_sta(const char *ssid, const char *password) {
  wifi_config_t wifi_config = {};
  std::strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid, sizeof(wifi_config.sta.ssid) - 1);
  std::strncpy(reinterpret_cast<char *>(wifi_config.sta.password), password, sizeof(wifi_config.sta.password) - 1);

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  return (esp_wifi_connect() == ESP_OK);
}

bool Wifi::start_softap(const char *ssid, const char *password, uint8_t channel, uint8_t max_conn) {
  wifi_config_t wifi_config = {};
  std::strncpy(reinterpret_cast<char *>(wifi_config.ap.ssid), ssid, sizeof(wifi_config.ap.ssid) - 1);
  wifi_config.ap.ssid_len = std::strlen(ssid);
  wifi_config.ap.channel = channel;
  std::strncpy(reinterpret_cast<char *>(wifi_config.ap.password), password, sizeof(wifi_config.ap.password) - 1);
  wifi_config.ap.max_connection = max_conn;
  wifi_config.ap.authmode = (std::strlen(password) == 0) ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  return true;
}

void Wifi::register_event_callback(WifiEventCallback cb) {
  m_event_cb = cb;
}

void Wifi::wifi_event_handler_proxy(void *arg, esp_event_base_t event_base,
                                           int32_t event_id, void *event_data) {
  auto *self = static_cast<Wifi *>(arg);
  if (self && self->m_event_cb) {
    self->m_event_cb(event_base, event_id, event_data);
  }
}