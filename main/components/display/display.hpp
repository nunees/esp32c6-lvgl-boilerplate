#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

esp_err_t display_init();
esp_lcd_panel_io_handle_t display_get_io();
esp_lcd_panel_handle_t display_get_panel();