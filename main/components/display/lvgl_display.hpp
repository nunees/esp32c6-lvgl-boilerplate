#ifndef LVGL_DISPLAY_HPP
#define LVGL_DISPLAY_HPP

#pragma once

#include "esp_err.h"
#include "display.hpp"
#include "lvgl.h"

class LvglDisplay
{
public:
    // Constructor allows passing custom resolutions and buffer rows if reused on different panels
    explicit LvglDisplay(Display& display, int width = 320, int height = 172, int buffer_rows = 40);
    ~LvglDisplay();

    esp_err_t init();
    lv_disp_t *get_display_handle() const;

private:
    static constexpr const char *TAG = "LvglDisplay";

    Display& display_;

    int width_;
    int height_;
    int buffer_rows_;

    
    lv_disp_t *m_disp_handle = nullptr;
};

#endif