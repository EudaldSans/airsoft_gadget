#pragma once

#include "Arduino.h"
#include "esp_err.h"
#include "menus.h"
#include <TFT_eSPI.h>

#define CURRENT_MENU_DEFAULT            AMMO_MENU
#define AMMO_MAG_SIZE_DEFAULT           30
#define AMMO_COUNT_SETTING_DEFAULT      DECREASE_MODE
#define CHRONO_UNIT_DEFAULT             CHRONO_FPS
#define CHRONO_BB_WEIGHT_DEFAULT        22
#define COLOR_0_DEFAULT                 TFT_GREEN
#define COLOR_1_DEFAULT                 TFT_YELLOW
#define COLOR_2_DEFAULT                 TFT_RED
#define COLOR_BG_DEFAULT                0x18E3

typedef enum {
    CFG_BOARD_INITIALIZED,
    CFG_CURRENT_MENU,
    CFG_AMMO_MAG_SIZE,
    CFG_AMMO_MAG_SIZE_MSB,
    CFG_AMMO_COUNT_SETTING,
    CFG_CHRONO_UNIT,
    CFG_CHRONO_BB_WEIGHT,
    CFG_COLOR_0,
    CFG_COLOR_0_MSB,
    CFG_COLOR_1,
    CFG_COLOR_1_MSB,
    CFG_COLOR_2,
    CFG_COLOR_2_MSB,
    CFG_COLOR_BG,
    CFG_COLOR_BG_MSB,

    LEN_CONFIGS

} config_t;


void init_config();

void update_config(config_t config, uint8_t value);
void update_word_config(config_t config, uint16_t value);
uint8_t get_config(config_t config);
uint16_t get_word_config(config_t config);

void save_all_configs();
