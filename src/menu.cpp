#include "menu.h"

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

#include "logo.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define FONT_75p BlackOpsOne75
#define FONT_28p BlackOpsOne28

#define SCREEN_CENTER      120
#define ARC_RADIOUS     120
#define ARC_START       30
#define ARC_END         330
#define ARC_THICKNESS   10

#define DARKER_GREY 0x18E3

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite large_font_spr = TFT_eSprite(&tft);
TFT_eSprite small_font_spr = TFT_eSprite(&tft);

Menu::Menu() {
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(DARKER_GREY);

    large_font_spr.loadFont(FONT_75p);
    large_font_spr.setColorDepth(16);
    small_font_spr.loadFont(FONT_28p);   
    small_font_spr.setColorDepth(16); 
}

void Menu::updateAmoCounter(uint16_t ammo, uint16_t color, bool init) {
    static uint16_t last_color = TFT_GREEN;
    uint16_t width, height;
    String text;

    if (color != last_color || init) {large_font_spr.setTextColor(color);}
    
    if (ammo >= 100) {text = " " + (String) ammo + " ";}
    else {text = "  " + (String) ammo + "  ";}

    width = large_font_spr.textWidth(text);
    height = large_font_spr.fontHeight();
    
    tft.setCursor(SCREEN_CENTER - width/2, SCREEN_CENTER - height/2);
    large_font_spr.printToSprite(text);
}

void Menu::updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init) {
    static uint16_t last_end_angle = 30, last_start_angle = 330;
    static uint16_t meter_last_color = TFT_GREEN;

    if (meter_last_color != color || init) {
        tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, new_end_angle, color, TFT_BLACK);

    } 
    
    if (new_end_angle > last_end_angle) {
        tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_end_angle, new_end_angle, color, TFT_BLACK);
    } else {
        tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_end_angle, last_end_angle, TFT_BLACK, DARKER_GREY);
    }

    if (new_start_angle < last_start_angle) {
        tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, new_start_angle, last_start_angle, color, TFT_BLACK);
    } else {
        tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, last_start_angle, new_start_angle, TFT_BLACK, DARKER_GREY);
    }

    last_end_angle = new_end_angle;
    last_start_angle = new_start_angle;
    meter_last_color = color;
}

void Menu::updateHeading(float heading, uint16_t color, bool init) {
    static uint16_t last_color = TFT_GREEN;
    uint16_t width, height;
    String text = "??";

    Serial.print("Updating heading: ");
    Serial.print(heading);
    Serial.print("\n");

    if (color != last_color || init) {small_font_spr.setTextColor(color);}

    if (heading <= 22.5 || heading > 337.5)         {text = "  N  ";}
    else if (heading > 22.5 && heading <= 67.5)     {text = "NE";}
    else if (heading > 67.5 && heading <= 112.5)    {text = "  E  ";}
    else if (heading > 112.5 && heading <= 157.5)   {text = "SE";}
    else if (heading > 157.5 && heading <= 202.5)   {text = "  S  ";}
    else if (heading > 202.5 && heading <= 247.5)   {text = "SW";}
    else if (heading > 247.5 && heading <= 292.5)   {text = "  W  ";}
    else if (heading > 292.5 && heading <= 337.5)   {text = "NW";}
    else {text = "ERR";}

    width = small_font_spr.textWidth(text);
    height = small_font_spr.fontHeight();

    tft.setCursor(SCREEN_CENTER - width/2, tft.height() - height);
    small_font_spr.printToSprite(text);
}

void update(screen_data_t data) {Serial.println("Called menu Update!");}

void btn0()         {Serial.println("Called menu btn0!");}
void btn1()         {Serial.println("Called menu btn1!");}
void btn2()         {Serial.println("Called menu btn2!");}

void scrollUp()     {Serial.println("Called menu ScrollUp!");}
void scrollDown()   {Serial.println("Called menu scrollDown!");}