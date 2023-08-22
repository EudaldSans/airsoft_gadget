#include "menus.h"
#include "config.h"

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

#include "logo.h"

#include <iomanip>
#include <sstream>

// The font names are arrays references, thus must NOT be in quotes ""
#define FONT_75p BlackOpsOne75
#define FONT_28p BlackOpsOne28

#define SCREEN_CENTER      120
#define ARC_RADIOUS     120
#define ARC_START       30
#define ARC_END         330
#define ARC_THICKNESS   10

Menu::Menu(TFT_eSPI* p_tft) {
    this->_tft = p_tft;

    this->_tft->loadFont(BlackOpsOne28);
    this->small_font_height = this->_tft->fontHeight();
    this->_tft->unloadFont(); 

    this->_tft->loadFont(BlackOpsOne75);
    this->large_font_height = this->_tft->fontHeight();
    this->_tft->unloadFont(); 
}

void Menu::updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init) {
    static uint16_t last_end_angle = 30, last_start_angle = 330;
    static uint16_t meter_last_color = get_word_config(CFG_COLOR_0);

    if (init) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, ARC_START, new_start_angle, TFT_BLACK, get_word_config(CFG_COLOR_BG)); 
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_end_angle, ARC_END, TFT_BLACK, get_word_config(CFG_COLOR_BG));
        goto cleanup;
    }

    if (new_end_angle <= last_start_angle || new_start_angle >= last_end_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, last_end_angle, TFT_BLACK, get_word_config(CFG_COLOR_BG));
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        goto cleanup;
    }

    if (new_end_angle < last_end_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_end_angle, last_end_angle, TFT_BLACK, get_word_config(CFG_COLOR_BG));
    }

    if (new_start_angle > last_start_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, new_start_angle, TFT_BLACK, get_word_config(CFG_COLOR_BG));
    }

    if (meter_last_color != color) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        goto cleanup;
    } 
    
    if (new_end_angle > last_end_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_end_angle, new_end_angle, color, TFT_BLACK);
    } 

    if (new_start_angle < last_start_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, last_start_angle, color, TFT_BLACK);
    } 

cleanup:
    last_end_angle = new_end_angle;
    last_start_angle = new_start_angle;
    meter_last_color = color;
}

void Menu::updateHeading(float heading, uint16_t color, bool init) {
    static uint16_t last_color = get_word_config(CFG_COLOR_0);
    static String previous_str = "";
    String str = "??";

    if (heading <= 22.5 || heading > 337.5)         {str = "  N  ";}
    else if (heading > 22.5 && heading <= 67.5)     {str = "NE";}
    else if (heading > 67.5 && heading <= 112.5)    {str = "  E  ";}
    else if (heading > 112.5 && heading <= 157.5)   {str = "SE";}
    else if (heading > 157.5 && heading <= 202.5)   {str = "  S  ";}
    else if (heading > 202.5 && heading <= 247.5)   {str = "SW";}
    else if (heading > 247.5 && heading <= 292.5)   {str = "  W  ";}
    else if (heading > 292.5 && heading <= 337.5)   {str = "NW";}
    else {str = "ERR";}

    if (color != last_color || init) {
        this->_tft->setTextColor(color, get_word_config(CFG_COLOR_BG));
        last_color = color;
    } else if (str == previous_str) {
        return;
    }

    this->_tft->loadFont(BlackOpsOne28);
    
    this->_tft->setCursor(SCREEN_CENTER, SCREEN_CENTER);
    this->_tft->setTextDatum(BC_DATUM);
    this->_tft->setTextColor(get_word_config(CFG_COLOR_BG));
    this->_tft->drawString(previous_str, SCREEN_CENTER, this->_tft->height());
    this->_tft->setTextColor(color);
    this->_tft->drawString(str, SCREEN_CENTER, this->_tft->height());

    this->_tft->unloadFont();

    previous_str = str;
}

void Menu::updateCentralText(String str, uint16_t color, bool init) {
    static uint16_t last_width = 0, last_color = get_word_config(CFG_COLOR_0);
    static String previous_str = "";
    
    if (color != last_color || init) {
        this->_tft->setTextColor(color, get_word_config(CFG_COLOR_BG));
        last_color = color;
    } else if (previous_str == str) {
        return;
    }

    this->_tft->loadFont(BlackOpsOne75);
    
    this->_tft->setCursor(SCREEN_CENTER, SCREEN_CENTER);
    this->_tft->setTextDatum(CC_DATUM);
    this->_tft->setTextColor(get_word_config(CFG_COLOR_BG));
    this->_tft->drawString(previous_str, SCREEN_CENTER, SCREEN_CENTER);
    this->_tft->setTextColor(color);
    this->_tft->drawString(str, SCREEN_CENTER, SCREEN_CENTER);

    this->_tft->unloadFont();

    previous_str = str;
}

void Menu::updateMenuTitle(String str, uint16_t color, bool init) {
    static uint16_t last_color = get_word_config(CFG_COLOR_0);
    static String previous_str = "";
    uint16_t width, height;
    
    if (color != last_color || init) {
        this->_tft->setTextColor(color, get_word_config(CFG_COLOR_BG));
        last_color = color;
    } else if (str == previous_str) {
        return;
    }

    this->_tft->loadFont(BlackOpsOne28);
    this->_tft->setCursor(SCREEN_CENTER, SCREEN_CENTER);
    this->_tft->setTextDatum(BC_DATUM);
    this->_tft->setTextColor(get_word_config(CFG_COLOR_BG));
    this->_tft->drawString(previous_str, SCREEN_CENTER, SCREEN_CENTER - (this->large_font_height / 2));
    this->_tft->setTextColor(color);
    this->_tft->drawString(str, SCREEN_CENTER, SCREEN_CENTER - (this->large_font_height / 2));

    this->_tft->unloadFont();

    previous_str = str;
}

void Menu::display_menu_activity(uint16_t color, bool init) {
    static uint16_t last_color = get_word_config(CFG_COLOR_0);
    if ((last_color == color && this->menu_active) || init);

    if (this->menu_active) {this->_tft->fillCircle(SCREEN_CENTER, 30, 10, color);}
    else {this->_tft->fillCircle(SCREEN_CENTER, 30, 10, get_word_config(CFG_COLOR_BG));}

    last_color = color;
}

void Menu::update(screen_data_t data, bool init) {
    Menu::updateMenuTitle(this->title, get_word_config(CFG_COLOR_2), true);
    Menu::updateCentralText("ERR", get_word_config(CFG_COLOR_2), true);
    Serial.println("Called menu Update!");
}

void Menu::clear() {Serial.println("Called menu clear!");}

void Menu::btn0()         {Serial.println("Called menu btn0!");}
void Menu::btn1()         {Serial.println("Called menu btn1!");}
void Menu::btn2()         {Serial.println("Called menu btn2!");}

void Menu::scrollUp()     {Serial.println("Called menu ScrollUp!");}
void Menu::scrollDown()   {Serial.println("Called menu scrollDown!");}

bool Menu::scrollKey()    {
    Serial.println("Called menu ScrollKey");
    this->menu_active = !this->menu_active;
    return this->menu_active;
}