#include "menus.h"
#include "config.h"

AmmoMenu::AmmoMenu(TFT_eSPI* p_tft):Menu(p_tft) {
    this->counting_mode = DECREASE_MODE;
}

void AmmoMenu::update(screen_data_t data, bool init) {
    static float previous_ratio = 1;
    static uint32_t color = get_word_config(CFG_COLOR_0);
    float current_ratio = (float)data.current_ammo / data.total_ammo;
    uint16_t meter_angle =  300 * current_ratio + 30;
    String ammo_text;

    switch (this->counting_mode) {
        case DECREASE_MODE: ammo_text = String(data.current_ammo, 10);                      break;
        case INCREASE_MODE: ammo_text = String(data.total_ammo - data.current_ammo, 10);    break;
        case TOTAL_MODE:    ammo_text = String(data.total_shots, 10);                       break;
        default:            ammo_text = "ERR";                                              break;
    }
    
    
    if (current_ratio > 0.5 && previous_ratio <= 0.5) { color = get_word_config(CFG_COLOR_0);} 
    else if (current_ratio > 0.25 && previous_ratio <= 0.5) {color = get_word_config(CFG_COLOR_1);} 
    else if (current_ratio < 0.25 && previous_ratio <= 0.25) {color = get_word_config(CFG_COLOR_2);} 
    
    Menu::updateArcMeter(ARC_START, meter_angle, color, init);
    Menu::updateCentralText(ammo_text, color, init);
    Menu::updateHeading(data.heading, color, init);  
    Menu::updateMenuTitle(this->title, color, init);
    Menu::display_menu_activity(color, init);

    previous_ratio = current_ratio;
}

void AmmoMenu::scrollUp() {
    this->counting_mode++;
    if (this->counting_mode >= NUMBER_OF_MODES) {this->counting_mode = 0;}
}

void AmmoMenu::scrollDown() {
    this->counting_mode--;
    if (this->counting_mode <= 0) {this->counting_mode = NUMBER_OF_MODES - 1;}
}

