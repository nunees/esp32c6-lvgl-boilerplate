#include "dashboard_screen.hpp"
#include <cstdio>

extern const lv_font_t fonts_awesome;
extern const lv_font_t lv_font_montserrat_34;

void DashboardScreen::create()
{
    // Create base screen container matching the landscape 320x172 dimensions
    screen = lv_obj_create(nullptr);
    lv_obj_set_size(screen, 320, 172);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // --- 1. Coolant Temperature (Top-Left Edge) ---
    coolant_label = lv_label_create(screen);
    lv_obj_set_style_text_font(coolant_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(coolant_label, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(coolant_label, LV_ALIGN_TOP_LEFT, 12, 8);
    lv_label_set_text(coolant_label, "--°C");

    // // --- 2. Lambda Sensor (Top-Right Edge) ---
    lambda_label = lv_label_create(screen);
    lv_obj_set_style_text_font(lambda_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lambda_label, lv_palette_main(LV_PALETTE_AMBER), 0);
    lv_obj_align(lambda_label, LV_ALIGN_TOP_RIGHT, -12, 8);
    lv_label_set_text(lambda_label, "AFR: --");

    // // --- 3. Speed (Center) ---
    speed_label = lv_label_create(screen);
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(speed_label, lv_color_white(), 0);
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(speed_label, &fonts_awesome, 0);
    lv_label_set_text(speed_label, "\xEF\x98\x93");

    // speed_label = lv_label_create(screen);

    // lv_label_set_text(speed_label, "0");

    // lv_obj_set_style_text_font(
    //     speed_label,
    //     &lv_font_montserrat_40,
    //     LV_PART_MAIN);

    // lv_obj_set_style_text_color(
    //     speed_label,
    //     lv_color_white(),
    //     LV_PART_MAIN);

    // // Calculate label dimensions before setting the pivot
    // lv_obj_update_layout(speed_label);

    // lv_obj_set_style_transform_pivot_x(
    //     speed_label,
    //     lv_obj_get_width(speed_label) / 2,
    //     LV_PART_MAIN);

    // lv_obj_set_style_transform_pivot_y(
    //     speed_label,
    //     lv_obj_get_height(speed_label) / 2,
    //     LV_PART_MAIN);

    // lv_obj_set_style_transform_angle(
    //     speed_label,
    //     900,
    //     LV_PART_MAIN);

    // lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);

    // // --- 4. Battery Voltage (Bottom Middle) ---
    // battery_label = lv_label_create(screen);
    // lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_14, 0);
    // lv_obj_set_style_text_color(battery_label, lv_palette_main(LV_PALETTE_GREEN), 0);
    // lv_obj_align(battery_label, LV_ALIGN_BOTTOM_MID, 0, -8);
    // lv_label_set_text(battery_label, "BAT: --V");
}

lv_obj_t *DashboardScreen::get_screen() const
{
    return screen;
}

void DashboardScreen::set_speed(uint16_t speed)
{
    if (speed_label == nullptr)
        return;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%u", speed);
    lv_label_set_text(speed_label, buffer);
}

void DashboardScreen::set_coolant_temp(float temp)
{
    if (coolant_label == nullptr)
        return;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "CLT: %.1f°C", temp);
    lv_label_set_text(coolant_label, buffer);
}

void DashboardScreen::set_lambda(float lambda)
{
    if (lambda_label == nullptr)
        return;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "AFR: %.2f", lambda);
    lv_label_set_text(lambda_label, buffer);
}

void DashboardScreen::set_battery_voltage(float voltage)
{
    if (battery_label == nullptr)
        return;
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "BAT: %.1fV", voltage);
    lv_label_set_text(battery_label, buffer);
}