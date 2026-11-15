#include "ui_task.hpp"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static UiTaskContext s_ctx{};

static void ui_refresh_task(void *arg) {
  auto *ctx = static_cast<UiTaskContext *>(arg);
  Datalink::EcuTelemetryData latest_data{};

  while (true) {
    ESP_LOGI(TAG, "UI Task is running!");
    if (ctx && ctx->telemetry_store && ctx->screen_manager) {
      if (ctx->telemetry_store->get(latest_data)) {
        if (lvgl_port_lock(0)) {
          ESP_LOGI(TAG, "Data: %d", latest_data.engine_RPM);
          ctx->screen_manager->update_dashboard_telemetry(latest_data);
          lvgl_port_unlock();
        }
      }
    }

    // ESP_LOGI(TAG, "Data: %d", latest_data.engine_RPM);

    // Refresh UI at ~30Hz (every 33ms)
    vTaskDelay(pdMS_TO_TICKS(33));
  }
}

esp_err_t start_ui_refresh_task(ScreenManager *screen_manager,
                                TelemetryStore *telemetry_store) {
  if (!screen_manager || !telemetry_store) {
    return ESP_ERR_INVALID_ARG;
  }

  s_ctx.screen_manager = screen_manager;
  s_ctx.telemetry_store = telemetry_store;

  BaseType_t res =
      xTaskCreate(ui_refresh_task, "ui_refresh_task", 4096, &s_ctx, 4, nullptr);

  if (res != pdPASS) {
    ESP_LOGE(TAG, "Failed to create UI refresh task");
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "UI refresh task started");
  return ESP_OK;
}
