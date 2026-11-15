#pragma once

#include <cstdint>
#include "esp_err.h"

esp_err_t backlight_init();
void backlight_set_brightness(uint8_t brightness);