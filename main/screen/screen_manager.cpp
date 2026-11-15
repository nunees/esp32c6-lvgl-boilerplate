#include "screen_manager.hpp"
#include "esp_log.h"

void ScreenManager::init()
{
    create_screens();

    lv_scr_load(screens[current_screen]);
}

void ScreenManager::create_screens()
{
    // Screen 1
    screens[0] = lv_obj_create(nullptr);

    lv_obj_t* label1 = lv_label_create(screens[0]);
    lv_label_set_text(label1, "DASHBOARD");
    lv_obj_center(label1);

    // Screen 2
    screens[1] = lv_obj_create(nullptr);

    lv_obj_t* label2 = lv_label_create(screens[1]);
    lv_label_set_text(label2, "DIAGNOSTICS");
    lv_obj_center(label2);

    // Screen 3
    screens[2] = lv_obj_create(nullptr);

    lv_obj_t* label3 = lv_label_create(screens[2]);
    lv_label_set_text(label3, "SETTINGS");
    lv_obj_center(label3);
}

void ScreenManager::next_screen()
{
    // current_screen++;

    // if (current_screen >= SCREEN_COUNT) {
    //     current_screen = 0;
    // }

    // lv_scr_load_anim(
    //     screens[current_screen],
    //     LV_SCR_LOAD_ANIM_MOVE_LEFT,
    //     250,
    //     0,
    //     false
    // );

    uint8_t next = (current_screen + 1) % SCREEN_COUNT;

    if (screens[next] == nullptr) {
        ESP_LOGE("SCREEN", "Screen %d is NULL", next);
        return;
    }

    current_screen = next;

    lv_scr_load_anim(
        screens[current_screen],
        LV_SCR_LOAD_ANIM_MOVE_LEFT,
        250,
        0,
        false
    );
}