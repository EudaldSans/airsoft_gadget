#include "config.h"
#include "EEPROM.h"
#include <esp_log.h>


void init_config() {
    log_i("Starting EEPROM");
    EEPROM.begin(LEN_CONFIGS);
    log_i("EEPROM length: %d", EEPROM.length());

    if (EEPROM.read(0) == LEN_CONFIGS) {
        log_i("Flashing EEPROM with default configs");
        update_config(CFG_BOARD_INITIALIZED, LEN_CONFIGS);
        update_config(CFG_CURRENT_MENU, CURRENT_MENU_DEFAULT);
        update_word_config(CFG_AMMO_MAG_SIZE, AMMO_MAG_SIZE_DEFAULT);
        update_config(CFG_AMMO_COUNT_SETTING, CHRONO_UNIT_DEFAULT);
        update_config(CFG_CHRONO_UNIT, CHRONO_UNIT_DEFAULT);
        update_config(CFG_CHRONO_BB_WEIGHT, CHRONO_BB_WEIGHT_DEFAULT);
        update_word_config(CFG_COLOR_0, COLOR_0_DEFAULT);
        update_word_config(CFG_COLOR_1, COLOR_1_DEFAULT);
        update_word_config(CFG_COLOR_2, COLOR_2_DEFAULT);
        update_word_config(CFG_COLOR_BG, COLOR_BG_DEFAULT);
        EEPROM.commit();
    }

    log_i("Configuration initialized");
}

void update_config(config_t config, uint8_t value) {
    EEPROM.write(config, value);
}

uint8_t get_config(config_t config) {
    return EEPROM.read(config);
}

void update_word_config(config_t config, uint16_t value) {
    EEPROM.write(config, (value & 0xFF));
    EEPROM.write(config + 1, value >> 8);
}

uint16_t get_word_config(config_t config) {
    return (EEPROM.read(config + 1) << 8) | EEPROM.read(config);
}

void save_all_configs() {
    EEPROM.commit();
}

