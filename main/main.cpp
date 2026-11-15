#include "esp_log.h"
#include "nvs_flash.h"
#include "wifi.hpp"

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

#include "app.hpp"

static ScreenManager screen_manager;

// Keep track of the active peer MAC once paired (or use broadcast / hardcoded target MAC)
// static uint8_t targetPeerMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// static bool peerConnected = false;

extern "C" void app_main()
{

    static App app;

    ESP_LOGI("MAIN", "Calling App::run()");

    app.run();

    if (lvgl_port_lock(0))
    {
        screen_manager.init();

        lvgl_port_unlock();
    };

    ESP_ERROR_CHECK(btn_init(&screen_manager)); //
}