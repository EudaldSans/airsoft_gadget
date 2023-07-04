#include "menus.h"

typedef enum {
    CHRONO_MPS = 0,
    CHRONO_FPS,
    NUMBER_OF_UNITS
} chrono_units_t;

ChronoMenu::ChronoMenu(TFT_eSPI* p_tft):Menu(p_tft) {
    this->units = CHRONO_MPS;
    ChronoMenu::_tft = p_tft;
}

void ChronoMenu::update(screen_data_t data, bool init) {
    static float previous_ratio = 1;
    static uint32_t color = TFT_GREEN;
    float current_ratio = (float) data.speed/data.max_speed;
    String chrono_text;
    int16_t meter_angle =  (float) 2417 * current_ratio - 2145;
    uint16_t speed = data.speed;

    if      (meter_angle < 30) {meter_angle = 30;}
    else if (meter_angle > ARC_END) {meter_angle = ARC_END;}

    if      (this->units == CHRONO_FPS) {speed = speed * 3.28084;}

    if      (speed == 0) {chrono_text = "--";}
    else    {chrono_text = String(speed, 10);}
    
    if      (current_ratio < 0.9) {color = TFT_GREEN;} 
    else if (current_ratio >= 0.95 && current_ratio <= 1) {color = TFT_YELLOW;} 
    else    {color = TFT_RED;} 
    
    Menu::updateArcMeter(ARC_START, meter_angle, color, init);
    Menu::updateCentralText(chrono_text, color, init);
    Menu::updateHeading(data.heading, color, init);  
    Menu::updateMenuTitle(this->title, color, init);

    ChronoMenu::_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 247, 248, TFT_RED, TFT_BLACK);

    previous_ratio = current_ratio;
}

void ChronoMenu::btn1() {
    this->units++;
    if (this->units >= NUMBER_OF_UNITS) {this->units = 0;}
}

void ChronoMenu::btn2() {
    this->units--;
    if (this->units <= 0) {this->units = NUMBER_OF_UNITS - 1;}
}