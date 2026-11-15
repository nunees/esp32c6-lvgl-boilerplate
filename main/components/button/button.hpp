#pragma once

#include "esp_err.h"
#include "screen_manager.hpp"

void btn_task(void* arg);
esp_err_t btn_init(ScreenManager* screen_manager);