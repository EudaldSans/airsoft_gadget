// This is a test sketch being developed for a new arc based meter widget
// The meter graphic is fully anti-aliased to avoid jaggy pixelated edges

// For this demo randomly sized meters are drawn, cycled and redrawn a random size.
// The meter is ramped up and down 0-100 and 100-0, then pauses before a new
// random sized meter is drawn

// If the radius is > 25 then the value is drawn in the middle

// The outer ring of the meter uses the drawSmoothCircle function (which draws
// a narrow full circle smooth arc)

// Uncomment to draw meter digits and label text
//#define DRAW_DIGITS

// If DRAW_DIGITS is defined the OpenFontRender library must be loaded since
// the sketch uses a scaleable TrueType font for the text and numerals.
// https://github.com/Bodmer/OpenFontRender

#define LOOP_DELAY 0 // This controls how frequently the meter is updated
                     // for test purposes this is set to 0


#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "Wire.h"
#include "I2Cdev.h"
#include <HMC5883L.h>

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

#define DARKER_GREY 0x18E3

// The font names are arrays references, thus must NOT be in quotes ""
#define FONT_75p BlackOpsOne75
#define FONT_28p BlackOpsOne28

#define SCREEN_CENTER      120
#define ARC_RADIOUS     120
#define ARC_START       30
#define ARC_END         330
#define ARC_THICKNESS   10

#define FONT_WIDTH      21  // in pixels

TFT_eSPI tft = TFT_eSPI();            // Invoke custom library with default width and height
TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object
uint16_t total_ammo = 30;
uint16_t meter_last_angle = 30, ammo = total_ammo;

HMC5883L mag;
int16_t mx, my, mz;


void ammoMeterUpdateValue(uint16_t ammo);
void ammoMeterInit();
void ammoMeterUpdateColor(uint32_t color);

void ammoCounterUpdate(uint16_t ammo);

void updateHeading(float heading);

void IRAM_ATTR shot_detected_ISR(void);

void setup(void) {
    Serial.begin(115200);
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_NAVY);
    //tft.setViewport(0, 0, 240, 320);
    spr.setColorDepth(16); // 16 bit colour needed to show anti-aliased fonts
    spr.setTextColor(TFT_GREEN, DARKER_GREY);


    ammoMeterInit();

    Wire.setPins(22, 21);
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

    attachInterrupt(15, shot_detected_ISR, FALLING);
}


void loop() {
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
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    heading = heading * 180/M_PI;

    mag.getHeading(&mx, &my, &mz);
    updateHeading(heading);    

    previous_ratio = current_ratio;

    delay(200);
    if (ammo > total_ammo) {ammo = total_ammo;}
}

void IRAM_ATTR shot_detected_ISR(void) {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 100) {
        ammo--;
        last_interrupt_time = interrupt_time;
    }
        
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
