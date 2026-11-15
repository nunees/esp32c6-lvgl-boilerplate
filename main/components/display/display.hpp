// #pragma once

// #include "esp_err.h"
// #include "esp_lcd_panel_io.h"
// #include "esp_lcd_panel_ops.h"

// esp_err_t display_init();
// esp_lcd_panel_io_handle_t display_get_io();
// esp_lcd_panel_handle_t display_get_panel();

#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

class Display
{
public:
    Display() = default;
    Display(const Display &) = delete;
    Display &operator=(const Display &) = delete;

    esp_err_t init();

    // LCD handles
    esp_lcd_panel_io_handle_t get_io() const;
    esp_lcd_panel_handle_t get_panel() const;

private:
    // Display resolution
    static constexpr int LCD_HRES = 172;
    static constexpr int LCD_VRES = 320;

    // GPIO configuration
    static constexpr int LCD_PIN_MOSI = 6;
    static constexpr int LCD_PIN_SCLK = 7;
    static constexpr int LCD_PIN_CS = 14;
    static constexpr int LCD_PIN_DC = 15;
    static constexpr int LCD_PIN_RST = 21;
    static constexpr int LCD_PIN_BL = 22;

    // Hardware handles
    esp_lcd_panel_io_handle_t lcd_io_ = nullptr;
    esp_lcd_panel_handle_t lcd_panel_ = nullptr;

    bool initialized_ = false;
};
