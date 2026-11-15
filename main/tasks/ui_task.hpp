#pragma once

#include "esp_err.h"
#include "screen_manager.hpp"
#include "telemetry_store.hpp"

struct UiTaskContext {
  ScreenManager *screen_manager;
  TelemetryStore *telemetry_store;
};

esp_err_t start_ui_refresh_task(ScreenManager *screen_manager,
                                TelemetryStore *telemetry_store);
