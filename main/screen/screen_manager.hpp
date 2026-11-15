// #pragma once

// #include "lvgl.h"
// #include "dashboard_screen.hpp"

// class ScreenManager {
// public:
//     void init();
//     void next_screen();

// private:
//     static constexpr uint8_t SCREEN_COUNT = 3;

//     DashboardScreen dashboard;
//     uint8_t current_screen = 0;

//     lv_obj_t* screens[SCREEN_COUNT] = {
//         nullptr,
//         nullptr,
//         nullptr
//     };

//     void create_screens();
// };


#pragma once

#include "lvgl.h"
#include "dashboard_screen.hpp"

class ScreenManager {
public:
    void init();
    void next_screen();

private:
    // Total screens: Dashboard (0), Diagnostics (1), Settings (2)
    static constexpr uint8_t TOTAL_SCREENS = 3;

    DashboardScreen dashboard;
    uint8_t current_screen = 0; // 0 = Dashboard, 1 = Diagnostics, 2 = Settings

    lv_obj_t* sub_screens[2] = {nullptr, nullptr}; // Diagnostics & Settings

    void create_sub_screens();
};