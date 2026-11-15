idf_component_register(
    SRCS
        "main.cpp"
        "display/display.cpp"
        "display/lvgl_display.cpp"

    INCLUDE_DIRS
        "."
        "display"

    REQUIRES
        esp_lcd
        esp_driver_spi
        esp_driver_ledc
        esp_lvgl_port
        lvgl
)