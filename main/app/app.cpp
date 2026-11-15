#include "app.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_now.h"
#include "nvs_flash.h"

#include "backlight.hpp"
#include "datalink.hpp"
#include "display.hpp"
#include "esp_lvgl_port.h"
#include "lvgl_display.hpp"
#include "screen_manager.hpp"
#include "wifi.hpp"

#include "encryption.hpp"

#include <cstring>

#include "telemetry_store.hpp"
#include "ui_task.hpp"

// Hardware and communication objects
static Wifi wifi;
static Datalink datalink;
static Backlight backlight;
static Display display;
static LvglDisplay
    lvgl_display(display); // Fixed: LvglDisplay constructor takes no arguments

static ScreenManager screen_manager;
static TelemetryStore telemetry_store;

// Datalink state
static uint8_t targetPeerMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bool peerConnected = false;

// ============================================================
// Wi-Fi initialization
// ============================================================

esp_err_t App::init_wifi() {
  ESP_LOGI(TAG, "Initializing Wi-Fi...");

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  // Initialize default event loop
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Initialize Wi-Fi
  ESP_ERROR_CHECK(wifi.init());

  // Register Wi-Fi events
  wifi.register_event_callback(
      [](esp_event_base_t base, int32_t id, void *data) {
        if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
          ESP_LOGI(TAG, "Wi-Fi Station started");
        }
      });

  ESP_LOGI(TAG, "Wi-Fi initialized");

  return ESP_OK;
}

// ============================================================
// Graphics initialization
// ============================================================

esp_err_t App::init_graphics() {
  ESP_LOGI(TAG, "Initializing graphics...");

  // Initialize LCD display
  ESP_ERROR_CHECK(display.init());

  // Initialize backlight
  backlight.init();
  backlight.set_brightness(1);

  // Initialize LVGL
  ESP_ERROR_CHECK(lvgl_display.init());

  // screen_manager.init();

  ESP_LOGI(TAG, "Graphics initialized");

  return ESP_OK;
}

// ============================================================
// Datalink initialization
// ============================================================

esp_err_t App::init_datalink() {
  // Initialize ESP-NOW
  datalink.init();

  // Configure primary master key
  datalink.set_primary_master_key(CUSTOM_PMK);

  // --------------------------------------------------------
  // Handshake callback
  // --------------------------------------------------------
  ESP_LOGI(TAG, "Trying handshake!");
  datalink.register_handshake_callback(
      [](const uint8_t *mac, const Datalink::HandshakePayload &hs) {
        ESP_LOGI(TAG, "Pairing device...");

        if (hs.magic_token == PAIRING_TOKEN) {
          ESP_LOGI(TAG, "Valid handshake received from Device ID: %d",
                   hs.device_id);

          std::memcpy(targetPeerMac, mac, 6);

          ESP_ERROR_CHECK(datalink.add_secure_peer(targetPeerMac, CUSTOM_LMK));

          peerConnected = true;

          datalink.send_handshake_ack(targetPeerMac, PAIRING_TOKEN, 0x01);
        }
      });

  // --------------------------------------------------------
  // Telemetry callback
  // --------------------------------------------------------

  // datalink.register_telemetry_callback(
  //     [this](const uint8_t *mac, const Datalink::EcuTelemetryData &telemetry)
  //     {
  //       ESP_LOGI("TELEMETRY", "RPM: %u | Speed: %u | Coolant: %.1f C",
  //                telemetry.engine_RPM, telemetry.speed,
  //                telemetry.coolant_temperature);
  //
  //       // 1. Store the incoming data safely
  //       telemetry_store.update(telemetry);
  //
  //       // CRITICAL: Lock LVGL before updating screen elements from a
  //       // network/callback task
  //       if (lvgl_port_lock(0)) {
  //         screen_manager.update_dashboard_telemetry(telemetry);
  //         lvgl_port_unlock();
  //       }
  //     });

  // --------------------------------------------------------
  // Send status callback
  // --------------------------------------------------------

  // datalink.register_send_status_callback(
  //     [](const uint8_t *mac, esp_now_send_status_t status) {
  //       if (status == ESP_NOW_SEND_SUCCESS) {
  //         ESP_LOGI(TAG, "Packet sent successfully!");
  //       } else {
  //         ESP_LOGW(TAG, "Packet delivery failed.");
  //       }
  //     });

  // --------------------------------------------------------
  // Register broadcast peer
  // --------------------------------------------------------

  datalink.add_secure_peer(broadcastMac, nullptr);

  // --------------------------------------------------------
  // Send handshake request
  // --------------------------------------------------------

  ESP_LOGI(TAG, "Sending handshake request...");

  bool sent =
      datalink.send_handshake_request(broadcastMac, PAIRING_TOKEN, 0x01);

  if (sent) {
    ESP_LOGI(TAG, "Handshake request sent successfully!");
  } else {
    ESP_LOGE(TAG, "Failed to send handshake request");
  }

  return ESP_OK;
}

// ============================================================
// Application entry point
// ============================================================

void App::run() {
  ESP_LOGI(TAG, "Starting application");

  ESP_LOGI(TAG, "Initializing Wi-Fi");
  ESP_ERROR_CHECK(init_wifi());

  ESP_LOGI(TAG, "Initializing datalink");
  ESP_ERROR_CHECK(init_datalink());

  ESP_LOGI(TAG, "Initializing graphics");
  ESP_ERROR_CHECK(init_graphics());

  ESP_LOGI(TAG, "Application initialized");

  if (lvgl_port_lock(0)) {
    screen_manager.init();
    ESP_ERROR_CHECK(start_ui_refresh_task(&screen_manager, &telemetry_store));

    lvgl_port_unlock();
  };
}
