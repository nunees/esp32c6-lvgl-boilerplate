#pragma once

#include "lvgl.h"

class ScreenManager {
public:
    void init();
    void next_screen();

private:
    static constexpr uint8_t SCREEN_COUNT = 3;

    uint8_t current_screen = 0;

    lv_obj_t* screens[SCREEN_COUNT] = {
        nullptr,
        nullptr,
        nullptr
    };

    void create_screens();
};