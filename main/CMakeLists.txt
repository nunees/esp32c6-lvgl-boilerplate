idf_component_register(
    SRCS
        "main.cpp"
        "components/display/display.cpp"
        "components/display/lvgl_display.cpp"
        "components/button/button.cpp"
        "screen/screen_manager.cpp"

    INCLUDE_DIRS
        "."
        "components/display"
        "components/button"
        "screen"

    REQUIRES
        esp_lcd
        esp_driver_spi
        esp_driver_ledc
        esp_lvgl_port
        lvgl
)