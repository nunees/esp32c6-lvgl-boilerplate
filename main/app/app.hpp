#ifndef APP_HPP
#define APP_HPP

#include "esp_check.h"
#include "screen_manager.hpp"
#include <cstdint>

class App
{
public:
    esp_err_t init_wifi();
    esp_err_t init_graphics();
    esp_err_t init_datalink();

    void run();

private:
    ScreenManager screen_manager;

    static constexpr uint8_t broadcastMac[6] = {
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF};
};

#endif