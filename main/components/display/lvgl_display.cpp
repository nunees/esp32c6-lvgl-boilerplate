#include "lvgl_display.hpp"
#include "display.hpp"

#include "esp_lvgl_port.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_check.h"
#include "esp_log.h"
#include "lvgl.h"

LvglDisplay::LvglDisplay(
    Display& display,
    int width,
    int height,
    int buffer_rows
)
    : display_(display),
      width_(width),
      height_(height),
      buffer_rows_(buffer_rows),
      m_disp_handle(nullptr)
{
}

LvglDisplay::~LvglDisplay() {
    // LVGL port lifecycle is usually persistent throughout application lifetime
}

esp_err_t LvglDisplay::init() {
    ESP_LOGI(TAG, "Initializing LVGL display subsystem (%dx%d)...", width_, height_);

    // Initialize LVGL and its background task
    const lvgl_port_cfg_t lvgl_config = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_RETURN_ON_ERROR(
        lvgl_port_init(&lvgl_config),
        TAG, "LVGL initialization failed");

    // Register the physical display with LVGL
    lvgl_port_display_cfg_t display_config = {};
    display_config.io_handle = display_.get_io();
    display_config.panel_handle = display_.get_panel();

    // Partial rendering buffer size configuration
    display_config.buffer_size = width_ * buffer_rows_;
    display_config.double_buffer = true;

    display_config.hres = width_;
    display_config.vres = height_;

    display_config.monochrome = false;

    display_config.rotation.swap_xy = true;
    display_config.rotation.mirror_x = false;
    display_config.rotation.mirror_y = true;

    display_config.flags.buff_dma = true;

    // Register display and obtain LVGL display handle
    m_disp_handle = lvgl_port_add_disp(&display_config);
    if (m_disp_handle == nullptr) {
        ESP_LOGE(TAG, "Failed to add LVGL display to port");
        return ESP_FAIL;
    }

    // Rotate the entire display 180 degrees
    lv_disp_set_rotation(m_disp_handle, LV_DISP_ROT_180);

    ESP_LOGI(TAG, "LVGL display initialized successfully");
    return ESP_OK;
}

lv_disp_t* LvglDisplay::get_display_handle() const {
    return m_disp_handle;
}