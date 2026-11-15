#pragma once

#include "lvgl.h"

class DashboardScreen
{
public:
    void create();

    lv_obj_t *get_screen() const;

    // Setter methods for real-time telemetry updates
    void set_speed(uint16_t speed);
    void set_coolant_temp(float temp);
    void set_lambda(float lambda);
    void set_battery_voltage(float voltage);

private:
    lv_obj_t* screen = nullptr;
    lv_obj_t* speed_label = nullptr;
    lv_obj_t* speed_label_km = nullptr;
    lv_obj_t* coolant_label = nullptr;
    lv_obj_t* coolant_icon = nullptr;
    lv_obj_t* lambda_label = nullptr;
    lv_obj_t* battery_label = nullptr;
};