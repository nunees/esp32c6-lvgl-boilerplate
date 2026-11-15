#include "display.hpp"

#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

static constexpr int LCD_HRES = 172;
static constexpr int LCD_VRES = 320;

static constexpr int LCD_PIN_MOSI = 6;
static constexpr int LCD_PIN_SCLK = 7;
static constexpr int LCD_PIN_CS   = 14;
static constexpr int LCD_PIN_DC   = 15;
static constexpr int LCD_PIN_RST  = 21;
static constexpr int LCD_PIN_BL   = 22;

static esp_lcd_panel_io_handle_t lcd_io = nullptr;
static esp_lcd_panel_handle_t lcd_panel = nullptr;

esp_err_t display_init()
{

    // 1. Configure and turn on the Backlight (GPIO22)
    gpio_config_t bk_gpio_config = {};
    bk_gpio_config.mode = GPIO_MODE_OUTPUT;
    bk_gpio_config.pin_bit_mask = (1ULL << LCD_PIN_BL);
    
    ESP_RETURN_ON_ERROR(
        gpio_config(&bk_gpio_config),
        "DISPLAY", "Backlight GPIO configuration failed"
    );
    
    // Set high to turn on backlight (or use LEDC PWM later for dimming)
    ESP_RETURN_ON_ERROR(
        gpio_set_level((gpio_num_t)LCD_PIN_BL, 1),
        "DISPLAY", "Backlight enable failed"
    );

    // SPI bus
    spi_bus_config_t bus_config = {};
    bus_config.mosi_io_num = LCD_PIN_MOSI;
    bus_config.miso_io_num = GPIO_NUM_NC;
    bus_config.sclk_io_num = LCD_PIN_SCLK;
    bus_config.quadwp_io_num = GPIO_NUM_NC;
    bus_config.quadhd_io_num = GPIO_NUM_NC;
    bus_config.max_transfer_sz = LCD_HRES * 40 * sizeof(uint16_t);

    ESP_RETURN_ON_ERROR(
        spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO),
        "DISPLAY", "SPI initialization failed"
    );

    // LCD communication
    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.cs_gpio_num = (gpio_num_t)LCD_PIN_CS;
    io_config.dc_gpio_num = (gpio_num_t)LCD_PIN_DC;
    io_config.spi_mode = 0;
    io_config.pclk_hz = 20 * 1000 * 1000;
    io_config.trans_queue_depth = 10;
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;

    ESP_RETURN_ON_ERROR(
        esp_lcd_new_panel_io_spi(
            SPI2_HOST,
            &io_config,
            &lcd_io
        ),
        "DISPLAY", "LCD IO initialization failed"
    );

    // ST7789 panel
    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
    panel_config.bits_per_pixel = 16;
    panel_config.reset_gpio_num = (gpio_num_t)LCD_PIN_RST;

    ESP_RETURN_ON_ERROR(
        esp_lcd_new_panel_st7789(
            lcd_io,
            &panel_config,
            &lcd_panel
        ),
        "DISPLAY", "ST7789 initialization failed"
    );

    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_reset(lcd_panel),
        "DISPLAY", "Panel reset failed"
    );

    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_init(lcd_panel),
        "DISPLAY", "Panel initialization failed"
    );

    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_invert_color(lcd_panel, true),
        "DISPLAY", "Panel inversion failed"
    );

    ESP_RETURN_ON_ERROR(
        esp_lcd_panel_disp_on_off(lcd_panel, true),
        "DISPLAY", "Panel enable failed"
    );

    return ESP_OK;
}

esp_lcd_panel_io_handle_t display_get_io()
{
    return lcd_io;
}

esp_lcd_panel_handle_t display_get_panel()
{
    return lcd_panel;
}