#include "ammo_screen.h"

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define FONT_75p BlackOpsOne75
#define FONT_28p BlackOpsOne28

#define SCREEN_CENTER      120
#define ARC_RADIOUS     120
#define ARC_START       30
#define ARC_END         330
#define ARC_THICKNESS   10

#define DARKER_GREY 0x18E3

TFT_eSPI tft = TFT_eSPI();            // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object
uint16_t total_ammo;
uint16_t meter_last_angle = 30, ammo;

void updateHeading(float heading);

void ammoCounterUpdate(uint16_t ammo);

void ammoMeterInit();
void ammoMeterUpdateValue(uint16_t ammo);
void ammoMeterUpdateColor(uint32_t color);

void ammoScreenUpdate(float heading) {
    static float previous_ratio = 1;
    float current_ratio = (float)ammo / total_ammo;

    ammoMeterUpdateValue(ammo);
    
    if (current_ratio > 0.5 && previous_ratio <= 0.5) {
        spr.setTextColor(TFT_GREEN, DARKER_GREY);
        ammoMeterUpdateColor(TFT_GREEN);
    } else if (current_ratio > 0.25 && previous_ratio <= 0.5) {
        spr.setTextColor(TFT_YELLOW, DARKER_GREY);
        ammoMeterUpdateColor(TFT_YELLOW);
    } else if (current_ratio < 0.25 && previous_ratio <= 0.25) {
        spr.setTextColor(TFT_RED, DARKER_GREY);
        ammoMeterUpdateColor(TFT_RED);
    }
    
    ammoCounterUpdate(ammo);
    updateHeading(heading);  

    previous_ratio = current_ratio;
}

void IRAM_ATTR ammoScreenDecreaseAmmo(void) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    if (interrupt_time - last_interrupt_time > 100) {
        if (ammo <= 0) {return;}
        ammo--;
        last_interrupt_time = interrupt_time;
    }
}

void IRAM_ATTR ammoScreenReload(void) {
    ammo = total_ammo;
}

void ammoScreenInit(uint16_t configured_ammo) {
    total_ammo = configured_ammo;
    ammo = total_ammo;

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_NAVY);
    //tft.setViewport(0, 0, 240, 320);
    spr.setColorDepth(16); // 16 bit colour needed to show anti-aliased fonts
    spr.setTextColor(TFT_GREEN, DARKER_GREY);

    ammoMeterInit();
}

void updateHeading(float heading) {
    uint16_t width, height;
    String text = "??";

    if (heading <= 22.5 || heading > 337.5)         {text = "  N  ";}
    else if (heading > 22.5 && heading <= 67.5)     {text = "NE";}
    else if (heading > 67.5 && heading <= 112.5)    {text = "  E  ";}
    else if (heading > 112.5 && heading <= 157.5)   {text = "SE";}
    else if (heading > 157.5 && heading <= 202.5)   {text = "  S  ";}
    else if (heading > 202.5 && heading <= 247.5)   {text = "SW";}
    else if (heading > 247.5 && heading <= 292.5)   {text = "  W  ";}
    else if (heading > 292.5 && heading <= 337.5)   {text = "NW";}
    else {text = "ERR";}

    spr.loadFont(FONT_28p);
    width = spr.textWidth(text);
    height = spr.fontHeight();

    tft.setCursor(SCREEN_CENTER - width/2, tft.height() - height);
    spr.printToSprite(text);
    spr.unloadFont();
}

void ammoCounterUpdate(uint16_t ammo) {
    uint16_t width, height;
    String text;
    
    if (ammo >= 100) {text = " " + (String) ammo + " ";}
    else {text = "  " + (String) ammo + "  ";}

    spr.loadFont(FONT_75p);
    width = spr.textWidth(text);
    height = spr.fontHeight();
    

    tft.setCursor(SCREEN_CENTER - width/2, SCREEN_CENTER - height/2);
    spr.printToSprite(text);
    spr.unloadFont();
}

void ammoMeterInit() {   
    tft.fillCircle(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, DARKER_GREY);
    // tft.drawSmoothCircle(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, TFT_SILVER, DARKER_GREY);
    uint16_t tmp = ARC_RADIOUS - 3;
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, ARC_START, ARC_END, TFT_BLACK, DARKER_GREY);
}

void ammoMeterUpdateValue(uint16_t ammo) {
    const int32_t x = tft.height()/2, y = tft.width()/2, r = tft.height() * 0.49;
    

    static uint32_t color = TFT_GREEN;

    uint16_t val_angle =  (300 * ammo) / total_ammo + 30;    

    // Update the arc, only the zone between last_angle and new val_angle is updated
    if (val_angle > meter_last_angle) {tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, meter_last_angle, val_angle, TFT_GREEN, TFT_BLACK);}
    else {tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, val_angle, meter_last_angle, TFT_BLACK, DARKER_GREY);}

    meter_last_angle = val_angle; // Store meter arc position for next redraw
}

void ammoMeterUpdateColor(uint32_t color) {
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, ARC_START, meter_last_angle, color, TFT_BLACK);
}