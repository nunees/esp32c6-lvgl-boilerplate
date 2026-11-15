
#include "display.hpp"

#include "driver/spi_common.h"
#include "esp_check.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"

#include "driver/gpio.h"

esp_err_t Display::init() {
  if (initialized_) {
    return ESP_OK;
  }

  // Backlight configuration
  gpio_config_t bk_gpio_config = {};
  bk_gpio_config.mode = GPIO_MODE_OUTPUT;
  bk_gpio_config.pin_bit_mask = (1ULL << LCD_PIN_BL);

  ESP_RETURN_ON_ERROR(gpio_config(&bk_gpio_config), "DISPLAY",
                      "Backlight GPIO configuration failed");

  ESP_RETURN_ON_ERROR(gpio_set_level(static_cast<gpio_num_t>(LCD_PIN_BL), 1),
                      "DISPLAY", "Backlight enable failed");

  // SPI bus configuration
  spi_bus_config_t bus_config = {};

  bus_config.mosi_io_num = LCD_PIN_MOSI;
  bus_config.miso_io_num = GPIO_NUM_NC;
  bus_config.sclk_io_num = LCD_PIN_SCLK;
  bus_config.quadwp_io_num = GPIO_NUM_NC;
  bus_config.quadhd_io_num = GPIO_NUM_NC;
  bus_config.max_transfer_sz = LCD_VRES * 40 * sizeof(uint16_t);

  ESP_RETURN_ON_ERROR(
      spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO), "DISPLAY",
      "SPI initialization failed");

  // LCD communication configuration
  esp_lcd_panel_io_spi_config_t io_config = {};

  io_config.cs_gpio_num = gpio_num_t(Display::LCD_PIN_CS);
  io_config.dc_gpio_num = gpio_num_t(Display::LCD_PIN_DC);
  io_config.spi_mode = 0;
  io_config.pclk_hz = 20 * 1000 * 1000;
  io_config.trans_queue_depth = 10;
  io_config.lcd_cmd_bits = 8;
  io_config.lcd_param_bits = 8;

  ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &lcd_io_),
                      "DISPLAY", "LCD IO initialization failed");

  // ST7789 configuration
  esp_lcd_panel_dev_config_t panel_config = {};

  panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
  panel_config.bits_per_pixel = 16;
  panel_config.reset_gpio_num = gpio_num_t(Display::LCD_PIN_RST);

  ESP_RETURN_ON_ERROR(
      esp_lcd_new_panel_st7789(lcd_io_, &panel_config, &lcd_panel_), "DISPLAY",
      "ST7789 initialization failed");

  // Reset panel
  ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(lcd_panel_), "DISPLAY",
                      "Panel reset failed");

  // Initialize panel
  ESP_RETURN_ON_ERROR(esp_lcd_panel_init(lcd_panel_), "DISPLAY",
                      "Panel initialization failed");

  // Display configuration
  ESP_RETURN_ON_ERROR(esp_lcd_panel_set_gap(lcd_panel_, 0, 34), "DISPLAY",
                      "Panel gap configuration failed");

  ESP_RETURN_ON_ERROR(esp_lcd_panel_swap_xy(lcd_panel_, true), "DISPLAY",
                      "Panel orientation failed");

  ESP_RETURN_ON_ERROR(esp_lcd_panel_mirror(lcd_panel_, true, false), "DISPLAY",
                      "Panel mirror configuration failed");

  ESP_RETURN_ON_ERROR(esp_lcd_panel_invert_color(lcd_panel_, true), "DISPLAY",
                      "Panel inversion failed");

  ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(lcd_panel_, true), "DISPLAY",
                      "Panel enable failed");

  initialized_ = true;

  return ESP_OK;
}

// Getters

esp_lcd_panel_io_handle_t Display::get_io() const { return lcd_io_; }

esp_lcd_panel_handle_t Display::get_panel() const { return lcd_panel_; }
