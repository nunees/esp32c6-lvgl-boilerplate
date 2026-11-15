#include "button.hpp"

#include "driver/gpio.h"
#include "esp_timer.h"

#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "screen_manager.hpp"

static constexpr gpio_num_t BUTTON_GPIO = GPIO_NUM_9;

static constexpr int64_t DEBOUNCE_US = 30000;

static int last_level = 1;
static int stable_level = 1;
static int64_t last_change_time = 0;

void btn_task(void *arg) {
  auto *screen_manager = static_cast<ScreenManager *>(arg);

  while (true) {
    int level = gpio_get_level(BUTTON_GPIO);
    int64_t now = esp_timer_get_time();

    if (level != last_level) {
      last_level = level;
      last_change_time = now;
    }

    if (level != stable_level && now - last_change_time >= DEBOUNCE_US) {

      stable_level = level;

      // Detect only the press transition
      if (stable_level == 0) {
        if (lvgl_port_lock(0)) {
          screen_manager->next_screen();
          lvgl_port_unlock();
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

esp_err_t btn_init(ScreenManager *screen_manager) {
  gpio_config_t config{};

  config.pin_bit_mask = 1ULL << BUTTON_GPIO;
  config.mode = GPIO_MODE_INPUT;
  config.pull_up_en = GPIO_PULLUP_ENABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;

  esp_err_t err = gpio_config(&config);

  if (err != ESP_OK) {
    return err;
  }

  xTaskCreate(btn_task, "button_task", 2048, screen_manager, 5, nullptr);

  return ESP_OK;
}
