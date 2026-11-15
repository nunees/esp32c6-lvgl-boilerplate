#include "esp_log.h"
#include "nvs_flash.h"

#include "display.hpp"
#include "lvgl_display.hpp"
#include "backlight.hpp"
#include "screen_manager.hpp"
#include "button.hpp"

#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "datalink.hpp"
#include "encryption.hpp"
#include "esp_wifi.h"
#include "esp_now.h"
#include <cstring>
#include <cstdio>

// DATALINK - CONFIG
// Use a global object or Datalink::get_instance()
static Datalink datalinkInstance;

// Keep track of the active peer MAC once paired (or use broadcast / hardcoded target MAC)
static uint8_t targetPeerMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static bool peerConnected = false;

// END DATALINK

static ScreenManager screen_manager;

static const char *TAG = "MAIN";

extern "C" void app_main()
{

    // --- 2. Initialize NVS (Required for Wi-Fi / ESP-NOW) ---
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // -------------------------------------------------------

    // 2. Initialize the Default Event Loop (Clears the Wi-Fi event post errors)
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(display_init());

    backlight_init();

    backlight_set_brightness(3); // 10%

    ESP_ERROR_CHECK(lvgl_display_init());

    if (lvgl_port_lock(0))
    {
        screen_manager.init();

        // 1. Initialize Wi-Fi and ESP-NOW via Datalink
        datalinkInstance.init(WIFI_MODE_STA);

        // 2. Set the Master Encryption Key (PMK)
        datalinkInstance.set_primary_master_key(CUSTOM_PMK);

        // 3. Register Handshake Callback for secure pairing
        datalinkInstance.register_handshake_callback([](const uint8_t *mac, const Datalink::HandshakePayload &hs)
                                                     {

                                                        ESP_LOGI(TAG, "Pairing device....");
        if (hs.magic_token == PAIRING_TOKEN) {
            ESP_LOGI("DATALINK", "Valid handshake received from Device ID: %d", hs.device_id);
            
            // Save peer MAC and add them as an encrypted secure peer with LMK
            std::memcpy(targetPeerMac, mac, 6);
            datalinkInstance.add_secure_peer(targetPeerMac, CUSTOM_LMK);
            peerConnected = true;

            // Acknowledge handshake if it was a request
            datalinkInstance.send_handshake_ack(targetPeerMac, PAIRING_TOKEN, 0x01);
        } });

        // 4. Register Telemetry Callback (Updates your LVGL UI elements live)
        datalinkInstance.register_telemetry_callback([](const uint8_t *mac, const Datalink::EcuTelemetryData &telemetry)
                                                     {
                                                         ESP_LOGI("TELEMETRY", "RPM: %u | Speed: %u | Coolant: %.1f°C",
                                                                  telemetry.engine_RPM, telemetry.speed, telemetry.coolant_temperature);

                                                         // NOTE: If speed_label and coolant_label are part of your ScreenManager or dashboard screen,
                                                         // you can expose update methods in your ScreenManager class to update them safely here.
                                                         // Example: screen_manager.update_telemetry(telemetry.speed, telemetry.coolant_temperature);
                                                     });

        // 5. Register Send Status feedback callback
        datalinkInstance.register_send_status_callback([](const uint8_t *mac, esp_now_send_status_t status)
                                                       {
        if (status == ESP_NOW_SEND_SUCCESS) {
            ESP_LOGI("DATALINK", "Packet sent successfully!");
        } else {
            ESP_LOGW("DATALINK", "Packet delivery failed.");
        } });


        // --- ADD THIS TO INITIATE THE HANDSHAKE ---
        uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        
        // 1. Add the broadcast address as a peer (lmk = nullptr means unencrypted for discovery)
        datalinkInstance.add_secure_peer(broadcastMac, nullptr);

        // 2. Send the handshake request packet out[cite: 1]
        bool sent = datalinkInstance.send_handshake_request(broadcastMac, PAIRING_TOKEN, 0x01);
        if (sent) {
            ESP_LOGI("DATALINK", "Handshake request sent successfully!");
        } else {
            ESP_LOGE("DATALINK", "Failed to send handshake request.");
        }
        // ------------------------------------------

        lvgl_port_unlock();
    }

    ESP_ERROR_CHECK(btn_init(&screen_manager));
}