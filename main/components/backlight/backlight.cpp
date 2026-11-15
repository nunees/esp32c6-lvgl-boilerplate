#include "display.hpp"

#include "driver/ledc.h"
#include "driver/gpio.h"

static constexpr gpio_num_t LCD_BL_PIN = GPIO_NUM_22;

static constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
static constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
static constexpr ledc_channel_t LEDC_CHANNEL = LEDC_CHANNEL_0;

esp_err_t backlight_init()
{
    ledc_timer_config_t timer = {};

    timer.speed_mode = LEDC_MODE;
    timer.duty_resolution = LEDC_TIMER_12_BIT;
    timer.timer_num = LEDC_TIMER;
    timer.freq_hz = 5000;
    timer.clk_cfg = LEDC_AUTO_CLK;

    esp_err_t err = ledc_timer_config(&timer);
    if (err != ESP_OK) {
        return err;
    }

    ledc_channel_config_t channel = {};

    channel.gpio_num = LCD_BL_PIN;
    channel.speed_mode = LEDC_MODE;
    channel.channel = LEDC_CHANNEL;
    channel.intr_type = LEDC_INTR_DISABLE;
    channel.timer_sel = LEDC_TIMER;
    channel.duty = 4095;
    channel.hpoint = 0;

    return ledc_channel_config(&channel);
}

void backlight_set_brightness(uint8_t brightness)
{
    if (brightness > 100) {
        brightness = 100;
    }

    const uint32_t duty = (brightness * 4095) / 100;

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}