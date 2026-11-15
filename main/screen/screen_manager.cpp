
#include "screen_manager.hpp"
#include "esp_log.h"

static const char* TAG = "SCREEN_MANAGER";

void ScreenManager::init()
{
    dashboard.create();

    if (dashboard.get_screen() == nullptr)
    {
        ESP_LOGE(TAG, "Dashboard creation failed");
        return;
    }

    create_screens();

    current_screen = 0;

    // Load dashboard
    lv_scr_load(dashboard.get_screen());

    ESP_LOGI(TAG, "Dashboard initialized");
}

void ScreenManager::create_screens()
{
    for (int i = 0; i < SCREEN_COUNT; i++)
    {
        screens[i] = lv_obj_create(nullptr);

        if (screens[i] == nullptr)
        {
            ESP_LOGE(TAG, "Failed to create screen %d", i);
            return;
        }
    }

    // Screen 1: Diagnostics
    lv_obj_t* label1 = lv_label_create(screens[0]);
    lv_label_set_text(label1, "DIAGNOSTICS");
    lv_obj_center(label1);

    // Screen 2: Settings
    lv_obj_t* label2 = lv_label_create(screens[1]);
    lv_label_set_text(label2, "SETTINGS");
    lv_obj_center(label2);
}

void ScreenManager::next_screen()
{
    uint8_t next = (current_screen + 1) % SCREEN_COUNT;

    if (screens[next] == nullptr)
    {
        ESP_LOGE(TAG, "Screen %d is NULL", next);
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
