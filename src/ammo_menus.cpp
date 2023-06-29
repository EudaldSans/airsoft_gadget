#include "menus.h"

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

#include "logo.h"


DecreasingAmmoMenu::DecreasingAmmoMenu():Menu() {}

void DecreasingAmmoMenu::update(screen_data_t data, bool init) {
    static float previous_ratio = 1;
    static uint32_t color = TFT_GREEN;
    float current_ratio = (float)data.current_ammo / data.total_ammo;
    uint16_t meter_angle =  300 * current_ratio + 30;
    
    if (current_ratio > 0.5 && previous_ratio <= 0.5) { color = TFT_GREEN;} 
    else if (current_ratio > 0.25 && previous_ratio <= 0.5) {color = TFT_YELLOW;} 
    else if (current_ratio < 0.25 && previous_ratio <= 0.25) {color = TFT_RED;} 
    
    Menu::updateArcMeter(ARC_START, meter_angle, color, init);
    Menu::updateAmoCounter(data.current_ammo, color, init);
    Menu::updateHeading(data.heading, color, init);  

    previous_ratio = current_ratio;
}


IncreasingAmmoMenu::IncreasingAmmoMenu():Menu() {}

void IncreasingAmmoMenu::update(screen_data_t data, bool init) {
    static float previous_ratio = 1;
    static uint32_t color = TFT_GREEN;
    float current_ratio = (float)data.current_ammo / data.total_ammo;
    uint16_t meter_angle =  360 - (300 * current_ratio + 30);
    
    if (current_ratio > 0.5 && previous_ratio <= 0.5) { color = TFT_GREEN;} 
    else if (current_ratio > 0.25 && previous_ratio <= 0.5) {color = TFT_YELLOW;} 
    else if (current_ratio < 0.25 && previous_ratio <= 0.25) {color = TFT_RED;} 
    
    Menu::updateArcMeter(meter_angle, ARC_END, color, init);
    Menu::updateAmoCounter(data.total_ammo - data.current_ammo, color, init);
    Menu::updateHeading(data.heading, color, init);  

    previous_ratio = current_ratio;
}


