idf_component_register(
    SRCS
        "main.cpp"
        "components/display/display.cpp"
        "components/display/lvgl_display.cpp"
        "components/button/button.cpp"
        "components/backlight/backlight.cpp"
        "screen/screen_manager.cpp"
        "ui/dashboard_screen.cpp"
        "fonts/fonts_awesome.c"

    INCLUDE_DIRS
        "."
        "components/display"
        "components/button"
        "components/backlight"
        "screen"
        "ui"

    REQUIRES
        esp_timer
        esp_lcd
        esp_driver_spi
        esp_driver_ledc
        esp_lvgl_port
        lvgl
)