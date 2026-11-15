#include "dashboard_screen.hpp"
#include <cstdio>

extern const lv_font_t fonts_awesome_34;
extern const lv_font_t lv_font_montserrat_34;
extern const lv_font_t big_digits_72;
extern const lv_font_t big_digits_100;

void DashboardScreen::create()
{
    // Create base screen container matching the landscape 320x172 dimensions
    screen = lv_obj_create(nullptr);
    lv_obj_set_size(screen, 320, 172);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // COOLANT
    // 1. Create the icon first (appears on the left)
    coolant_icon = lv_label_create(screen);
    lv_obj_set_style_text_font(coolant_icon, &fonts_awesome_34, 0);
    lv_obj_set_style_text_color(coolant_icon, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(coolant_icon, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(coolant_icon, "\xEF\x9D\xA9"); // fa-temperature-high

    // 2. Create the value label on its side (to the right)
    coolant_label = lv_label_create(screen);
    lv_obj_set_style_text_font(coolant_label, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(coolant_label, lv_palette_main(LV_PALETTE_RED), 0);
    // Align to the right of the icon with a small 6-pixel gap, centered vertically relative to the icon
    lv_obj_align_to(coolant_label, coolant_icon, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
    lv_label_set_text(coolant_label, "14");

    // // --- 2. Lambda Sensor (Top-Right Edge) ---
    lambda_label = lv_label_create(screen);
    lv_obj_set_style_text_font(lambda_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lambda_label, lv_palette_main(LV_PALETTE_AMBER), 0);
    lv_obj_align(lambda_label, LV_ALIGN_TOP_RIGHT, -12, 8);
    lv_label_set_text(lambda_label, "AFR: --");

    // // --- 3. Speed (Center) ---
    speed_label = lv_label_create(screen);
    // lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(speed_label, lv_color_white(), 0);
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(speed_label, &big_digits_100, 0);
    set_speed(50);

    speed_label_km = lv_label_create(screen);
    lv_obj_set_style_text_font(speed_label_km, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(speed_label_km, lv_color_white(), 0);
    lv_obj_align(speed_label_km, LV_ALIGN_CENTER, 0, 50);
    lv_label_set_text(speed_label_km, "Km/h");

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