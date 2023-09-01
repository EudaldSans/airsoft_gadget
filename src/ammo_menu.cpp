#include "menus.h"
#include "config.h"
#include "arrows.h"

AmmoMenu::AmmoMenu(TFT_eSPI* p_tft):Menu(p_tft) {
    this->counting_mode = get_config(CFG_AMMO_COUNT_SETTING);
    this->current_ammo = get_word_config(CFG_AMMO_MAG_SIZE);
    this->total_shots = 0;
}

void AmmoMenu::update(float heading, bool init) {
    static float previous_ratio = 1;
    static uint32_t color = get_word_config(CFG_COLOR_0);
    uint16_t total_ammo = get_word_config(CFG_AMMO_MAG_SIZE);
    float current_ratio = (float)this->current_ammo / total_ammo;
    uint16_t meter_width =  METER_WIDTH * current_ratio;
    String ammo_text;

    switch (this->counting_mode) {
        case DECREASE_MODE: ammo_text = String(this->current_ammo, 10);                     break;
        case INCREASE_MODE: ammo_text = String(total_ammo - this->current_ammo, 10);        break;
        case TOTAL_MODE:    ammo_text = String(this->total_shots, 10);                      break;
        default:            ammo_text = "ERR";                                              break;
    }
    
    
    if (current_ratio > 0.5 && previous_ratio <= 0.5) { color = get_word_config(CFG_COLOR_0);} 
    else if (current_ratio > 0.25 && previous_ratio <= 0.5) {color = get_word_config(CFG_COLOR_1);} 
    else if (current_ratio < 0.25 && previous_ratio <= 0.25) {color = get_word_config(CFG_COLOR_2);} 
    
    Menu::updateMeter(METER_START, meter_width, color, init);
    Menu::updateCentralText(ammo_text, color, init);
    Menu::updateHeading(heading, color, init);  
    Menu::updateMenuTitle(this->title, color, init);
    Menu::display_menu_activity(color, init);
    this->drawCountingMode(color, init);

    previous_ratio = current_ratio;
}

void AmmoMenu::drawCountingMode(uint16_t color, bool init) {
    static int8_t previous_counting_mode = 0;
    if ((previous_counting_mode == this->counting_mode) && !init) {return;}

    Serial.println("Updating mode sprite");

    Menu::_tft->fillRect(SCREEN_CENTER - ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_WIDTH - 5, ARROW_WIDTH, ARROW_HEIGHT, get_word_config(CFG_COLOR_BG));

    switch (this->counting_mode) {
        case DECREASE_MODE: Menu::_tft->drawXBitmap(SCREEN_CENTER - ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_WIDTH - 5, arrowDown, ARROW_WIDTH, ARROW_HEIGHT, color);       break;
        case INCREASE_MODE: Menu::_tft->drawXBitmap(SCREEN_CENTER - ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_WIDTH - 5, arrowUp, ARROW_WIDTH, ARROW_HEIGHT, color);         break;
        case TOTAL_MODE:    Menu::_tft->drawXBitmap(SCREEN_CENTER - ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_WIDTH - 5, doubleArrow, ARROW_WIDTH, ARROW_HEIGHT, color);     break;
        default:            Serial.println("ERROR, tried to paint invalid ammo counting mode");                                                 break;
    }    

    previous_counting_mode = this->counting_mode;
}

void AmmoMenu::clear() {
    Menu::_tft->fillRect(SCREEN_CENTER - ARROW_WIDTH/2, SCREEN_HEIGHT - ARROW_WIDTH - 5, ARROW_WIDTH, ARROW_HEIGHT, get_word_config(CFG_COLOR_BG));
}

void AmmoMenu::scrollUp() {
    this->counting_mode++;
    if (this->counting_mode >= NUMBER_OF_MODES) {this->counting_mode = 0;}
    update_config(CFG_AMMO_COUNT_SETTING, this->counting_mode);
}

void AmmoMenu::scrollDown() {
    this->counting_mode--;
    if (this->counting_mode < 0) {this->counting_mode = NUMBER_OF_MODES - 1;}
    update_config(CFG_AMMO_COUNT_SETTING, this->counting_mode);
}

void AmmoMenu::shot() {
    if (this->current_ammo > 0) {
        this->current_ammo--;
    }

    this->total_shots++;
}

