#include "esp_log.h"

#include "display.hpp"
#include "lvgl_display.hpp"
#include "backlight.hpp"
#include "screen_manager.hpp"
#include "button.hpp"

#include "esp_lvgl_port.h"
#include "lvgl.h"

static ScreenManager screen_manager;

extern "C" void app_main()
{
    ESP_ERROR_CHECK(display_init());

    backlight_init();

    backlight_set_brightness(3);  // 10%

    ESP_ERROR_CHECK(lvgl_display_init());

    if (lvgl_port_lock(0))
    {
        screen_manager.init();
        // lv_obj_t *screen = lv_scr_act();

        // lv_obj_t *label = lv_label_create(screen);

        // lv_label_set_text(
        //     label,
        //     "Hello LVGL!"
        // );

        // lv_obj_align(
        //     label,
        //     LV_ALIGN_CENTER,
        //     0,
        //     0
        // );

        lvgl_port_unlock();
    }

    ESP_ERROR_CHECK(btn_init(&screen_manager));
}