#ifndef ENCRYPTION_HPP
#define ENCRYPTION_HPP

#include "esp_now.h"

const uint8_t CUSTOM_PMK[ESP_NOW_KEY_LEN] = {
    'p', 'm', 'k', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '1', '2', '3'
};

const uint8_t CUSTOM_LMK[ESP_NOW_KEY_LEN] = {
    'l', 'm', 'k', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '1', '2', '3'
};

const uint32_t PAIRING_TOKEN = 0xCAFE10AD;

#endif