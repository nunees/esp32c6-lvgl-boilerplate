#include "lvgl_display.hpp"
#include "display.hpp"

#include "esp_lvgl_port.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#include "esp_check.h"

#include "lvgl.h"

static constexpr int LCD_WIDTH = 172;
static constexpr int LCD_HEIGHT = 320;

esp_err_t lvgl_display_init()
{
    // Initialize LVGL and its background task
    const lvgl_port_cfg_t lvgl_config = ESP_LVGL_PORT_INIT_CONFIG();

    ESP_RETURN_ON_ERROR(
        lvgl_port_init(&lvgl_config),
        "LVGL", "LVGL initialization failed"
    );

    // Register the physical display with LVGL
    lvgl_port_display_cfg_t display_config = {};
    display_config.io_handle = display_get_io();
    display_config.panel_handle = display_get_panel();
    
    // Partial rendering buffer: 40 rows
    display_config.buffer_size = LCD_WIDTH * 40;
    display_config.double_buffer = true;

    display_config.hres = LCD_WIDTH;
    display_config.vres = LCD_HEIGHT;

    display_config.monochrome = false;

    display_config.rotation.swap_xy = false;
    display_config.rotation.mirror_x = false;
    display_config.rotation.mirror_y = false;

    display_config.flags.buff_dma = true;

    if (lvgl_port_add_disp(&display_config) == nullptr) {
        return ESP_FAIL;
    }

    return ESP_OK;
}