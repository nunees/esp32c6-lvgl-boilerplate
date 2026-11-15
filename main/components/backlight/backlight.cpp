// #include "display.hpp"

// #include "driver/ledc.h"
// #include "driver/gpio.h"

// static constexpr gpio_num_t LCD_BL_PIN = GPIO_NUM_22;

// static constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
// static constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
// static constexpr ledc_channel_t LEDC_CHANNEL = LEDC_CHANNEL_0;

// esp_err_t backlight_init()
// {
//     ledc_timer_config_t timer = {};

//     timer.speed_mode = LEDC_MODE;
//     timer.duty_resolution = LEDC_TIMER_12_BIT;
//     timer.timer_num = LEDC_TIMER;
//     timer.freq_hz = 5000;
//     timer.clk_cfg = LEDC_AUTO_CLK;

//     esp_err_t err = ledc_timer_config(&timer);
//     if (err != ESP_OK) {
//         return err;
//     }

//     ledc_channel_config_t channel = {};

//     channel.gpio_num = LCD_BL_PIN;
//     channel.speed_mode = LEDC_MODE;
//     channel.channel = LEDC_CHANNEL;
//    // channel.intr_type = LEDC_INTR_DISABLE;
//     channel.timer_sel = LEDC_TIMER;
//     channel.duty = 4095;
//     channel.hpoint = 0;

//     return ledc_channel_config(&channel);
// }

// void backlight_set_brightness(uint8_t brightness)
// {
//     if (brightness > 100) {
//         brightness = 100;
//     }

//     const uint32_t duty = (brightness * 4095) / 100;

//     ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
//     ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
// }

#include "backlight.hpp"
#include "esp_log.h"

Backlight::Backlight(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer, ledc_mode_t speed_mode)
    : m_pin(pin),
      m_channel(channel),
      m_timer(timer),
      m_speed_mode(speed_mode),
      m_current_brightness(100) {}

Backlight::~Backlight()
{
    set_brightness(0); // Turn off backlight on destruction
}

esp_err_t Backlight::init()
{
    ESP_LOGI(TAG, "Initializing backlight on GPIO %d (LEDC Channel %d)...", m_pin, m_channel);

    ledc_timer_config_t timer_cfg = {};
    timer_cfg.speed_mode = m_speed_mode;
    timer_cfg.duty_resolution = LEDC_TIMER_12_BIT;
    timer_cfg.timer_num = m_timer;
    timer_cfg.freq_hz = 5000;
    timer_cfg.clk_cfg = LEDC_AUTO_CLK;

    esp_err_t err = ledc_timer_config(&timer_cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(err));
        return err;
    }

    ledc_channel_config_t channel_cfg = {};
    channel_cfg.gpio_num = m_pin;
    channel_cfg.speed_mode = m_speed_mode;
    channel_cfg.channel = m_channel;
    channel_cfg.timer_sel = m_timer;
    channel_cfg.duty = 4095; // Default full duty
    channel_cfg.hpoint = 0;

    err = ledc_channel_config(&channel_cfg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Backlight initialized successfully");
    return ESP_OK;
}

void Backlight::set_brightness(uint8_t brightness)
{
    if (brightness > 100)
    {
        brightness = 100;
    }

    m_current_brightness = brightness;

    const uint32_t duty = (brightness * 4095) / 100;

    ESP_LOGI(TAG, "Setting brightness to %d%% -> LEDC duty: %lu", brightness, duty);

    ledc_set_duty(m_speed_mode, m_channel, duty);
    ledc_update_duty(m_speed_mode, m_channel);
}

uint8_t Backlight::get_brightness() const
{
    return m_current_brightness;
}