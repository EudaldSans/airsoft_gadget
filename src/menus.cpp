#include "menus.h"

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

#define DARKER_GREY 0x18E3

Menu::Menu(TFT_eSPI* p_tft) {
    this->_tft = p_tft;
    this->_large_font_spr = new TFT_eSprite(p_tft);
    this->_small_font_spr = new TFT_eSprite(p_tft);

    this->_tft->begin();
    this->_tft->setRotation(1);
    this->_tft->fillScreen(DARKER_GREY);

    this->_large_font_spr->loadFont(FONT_75p);
    this->_large_font_spr->setColorDepth(16);
    this->_small_font_spr->loadFont(FONT_28p);   
    this->_small_font_spr->setColorDepth(16); 
}

void Menu::updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init) {
    static uint16_t last_end_angle = 30, last_start_angle = 330;
    static uint16_t meter_last_color = TFT_GREEN;

    if (init) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, ARC_START, new_start_angle, TFT_BLACK, DARKER_GREY); 
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_end_angle, ARC_END, TFT_BLACK, DARKER_GREY);
        goto cleanup;
    }

    if (new_end_angle <= last_start_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, last_end_angle, TFT_BLACK, DARKER_GREY);
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        goto cleanup;
    }

    if (new_start_angle >= last_end_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, last_end_angle, TFT_BLACK, DARKER_GREY);
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);
        goto cleanup;
    }

    if (new_end_angle < last_end_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_end_angle, last_end_angle, TFT_BLACK, DARKER_GREY);
    }

    if (new_start_angle > last_start_angle) {
        this->_tft->drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, new_start_angle, TFT_BLACK, DARKER_GREY);
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
    static uint16_t last_color = TFT_GREEN;
    uint16_t width, height;
    String text = "??";

    if (color != last_color || init) {
        this->_small_font_spr->setTextColor(color, DARKER_GREY);
        last_color = color;
    }

    if (heading <= 22.5 || heading > 337.5)         {text = "  N  ";}
    else if (heading > 22.5 && heading <= 67.5)     {text = "NE";}
    else if (heading > 67.5 && heading <= 112.5)    {text = "  E  ";}
    else if (heading > 112.5 && heading <= 157.5)   {text = "SE";}
    else if (heading > 157.5 && heading <= 202.5)   {text = "  S  ";}
    else if (heading > 202.5 && heading <= 247.5)   {text = "SW";}
    else if (heading > 247.5 && heading <= 292.5)   {text = "  W  ";}
    else if (heading > 292.5 && heading <= 337.5)   {text = "NW";}
    else {text = "ERR";}

    width = this->_small_font_spr->textWidth(text);
    height = this->_small_font_spr->fontHeight();

    this->_tft->setCursor(SCREEN_CENTER - width/2, this->_tft->height() - height);
    this->_small_font_spr->printToSprite(text);

}

void Menu::updateCentralText(String str, uint16_t color, bool init) {
    static uint16_t last_width = 0, last_color = TFT_GREEN;
    uint16_t width, height;
    
    if (color != last_color || init) {
        this->_large_font_spr->setTextColor(color, DARKER_GREY);
        last_color = color;
    }

    if (str.length() <= 1) {str = " " + str + " ";}

    width = this->_large_font_spr->textWidth(str);
    height = this->_large_font_spr->fontHeight();

    // TODO: Update only relevant boxes for optimisation
    if (last_width > width) {this->_tft->fillRect(SCREEN_CENTER - last_width/2, SCREEN_CENTER - height/2, last_width, height, DARKER_GREY);}
    last_width = width;

    this->_tft->setCursor(SCREEN_CENTER - width/2, SCREEN_CENTER - height/2);
    this->_large_font_spr->printToSprite(str);
}

void Menu::update(screen_data_t data, bool init) {
    Menu::updateCentralText("ERR", TFT_RED, true);
    Serial.println("Called menu Update!");
}

void Menu::btn0()         {Serial.println("Called menu btn0!");}
void Menu::btn1()         {Serial.println("Called menu btn1!");}
void Menu::btn2()         {Serial.println("Called menu btn2!");}

void Menu::scrollUp()     {Serial.println("Called menu ScrollUp!");}
void Menu::scrollDown()   {Serial.println("Called menu scrollDown!");}