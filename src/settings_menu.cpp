#include "menus.h"
#include "config.h"

#include "BlackOpsOne28.h"

#define MAX_SETTINGS    6

String setting_names[] = {
    "Ammo",
    "Chrono",
    "KDR",
    "Color",
    "Time",
    "<<<"
};


SettingsMenu::SettingsMenu(TFT_eSPI* p_tft):Menu(p_tft) {

}

void SettingsMenu::update(float heading, bool init) {  
    if (init) {
        this->_tft->fillScreen(get_word_config(CFG_COLOR_BG));
        this->current_setting = 0;
    }

    this->drawMainMenu(init);
}

void SettingsMenu::drawMainMenu(bool init) {
    uint8_t current_seting_pos;

    if (this->previous_setting == this->current_setting && !init) {return;} 

    this->_tft->loadFont(BlackOpsOne28);
    this->_tft->setTextDatum(TC_DATUM);

    if (this->menu_active) {
        current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * previous_setting;
        this->_tft->fillRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_BG));
        current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * this->current_setting;
        this->_tft->fillRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_0));
    }

    for (int i = 0; i < MAX_SETTINGS; i++) {
        current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * i;

        if (i == current_setting && this->menu_active) {
            this->_tft->setTextColor(get_word_config(CFG_COLOR_BG));
            this->_tft->drawString(setting_names[i], SCREEN_CENTER, current_seting_pos);
            
        } else {
            this->_tft->setTextColor(get_word_config(CFG_COLOR_0));
            this->_tft->drawString(setting_names[i], SCREEN_CENTER, current_seting_pos);
            this->_tft->drawRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_0));
        }
    }

    this->previous_setting = this->current_setting;
    
    this->_tft->unloadFont();
}



void SettingsMenu::clear() {
    this->_tft->fillScreen(get_word_config(CFG_COLOR_BG));
}

void SettingsMenu::scrollUp() {
    this->current_setting++;

    if (this->current_setting >= MAX_SETTINGS) {this->current_setting = 0;}
}

void SettingsMenu::scrollDown() {
    this->current_setting--;

    if (this->current_setting < 0) {this->current_setting = MAX_SETTINGS - 1;}
}

bool SettingsMenu::scrollKey() {
    this->previous_setting = 0xff;

    return Menu::scrollKey();
}
