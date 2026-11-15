#pragma once

#include "esp_err.h"
#include "hal/ledc_types.h"
#include "soc/gpio_num.h"
#include <cstdint>

class Backlight {
public:
  // Constructor allows passing custom pins and peripherals with safe defaults
  Backlight(gpio_num_t pin = GPIO_NUM_22,
            ledc_channel_t channel = LEDC_CHANNEL_0,
            ledc_timer_t timer = LEDC_TIMER_0,
            ledc_mode_t speed_mode = LEDC_LOW_SPEED_MODE);
  ~Backlight();

  esp_err_t init();
  void set_brightness(uint8_t brightness); // 0 to 100%
  uint8_t get_brightness() const;

private:
  static constexpr const char *TAG = "Backlight";

  gpio_num_t m_pin;
  ledc_channel_t m_channel;
  ledc_timer_t m_timer;
  ledc_mode_t m_speed_mode;
  uint8_t m_current_brightness;
};
