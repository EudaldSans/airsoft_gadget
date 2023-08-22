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

// #include "ammo_screen.h"

#include "Wire.h"
#include "I2Cdev.h"
#include <HMC5883L.h>

#include "TFT_eSPI.h"

#include "menus.h"
#include "RTX_logo.h"

#define ENCODER_KEY     15
#define ENCODER_1       23
#define ENCODER_2       19      

#define LONG_PRESS_TIME_MS      3000


TFT_eSPI tft = TFT_eSPI();

bool init_menu = true, inside_menu = false, long_press = true;

HMC5883L mag;

Menu* menus[NUMBER_OF_MENUS];
Menu* menu_to_clear = NULL;
uint8_t current_menu = CHRONO_MENU;

screen_data_t data = {
    .total_ammo = 30,
    .current_ammo = 30,
    .total_shots = 0,
    .speed = 355,
    .max_speed = 360,
    .heading = 0
};


void shot_detected_ISR(void);
void btn0_ISR(void); 

void encoder_1_ISR(void);
void encoder_2_ISR(void);
void encoder_key_event_ISR(void);

void setup(void) {
    Serial.begin(115200);
    Serial.println("Start");

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(DARKER_GREY);
    tft.drawXBitmap(SCREEN_CENTER - RTX_LOGO_W/2, SCREEN_CENTER - RTX_LOGO_H/2, RTX_logo_bitmap, RTX_LOGO_W, RTX_LOGO_H, TFT_RED);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 0, 30, TFT_RED, TFT_BLACK);

    // ammoScreenInit(30);

    Wire.setPins(22, 21);
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 30, 40, TFT_RED, TFT_BLACK);
    // delay(100);

    // initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 40, 50, TFT_RED, TFT_BLACK);
    // delay(100);

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 50, 70, TFT_RED, TFT_BLACK);
    // delay(100);
    
    menus[AMMO_MENU]        = new AmmoMenu(&tft);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 70, 100, TFT_RED, TFT_BLACK);
    // delay(100);
    menus[KDR_MENU]         = new KDRMenu(&tft);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 100, 130, TFT_RED, TFT_BLACK);
    // delay(100);
    menus[CHRONO_MENU]      = new ChronoMenu(&tft);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 130, 160, TFT_RED, TFT_BLACK);
    // delay(100);

    pinMode(ENCODER_KEY, INPUT_PULLUP);
    pinMode(ENCODER_1, INPUT);
    pinMode(ENCODER_2, INPUT);

    // attachInterrupt(15, shot_detected_ISR, FALLING);
    attachInterrupt(0, btn0_ISR, FALLING);
    attachInterrupt(ENCODER_KEY, encoder_key_event_ISR, CHANGE);
    attachInterrupt(ENCODER_1, encoder_1_ISR, FALLING);

    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 160, 190, TFT_RED, TFT_BLACK);
    // delay(100);

    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 190, 270, TFT_RED, TFT_BLACK);
    // delay(500);

    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - ARC_THICKNESS, 270, 360, TFT_RED, TFT_BLACK);
    // delay(500);

    tft.fillScreen(DARKER_GREY);
}


void loop() {
    int16_t mx, my, mz;
    
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    heading = heading * 180/M_PI;

    data.heading = heading;

    // dec_ammo_menu->update(data, false);
    if (menu_to_clear != NULL) {
        menu_to_clear->clear();
        menu_to_clear = NULL;
    }
    menus[current_menu]->update(data, init_menu);
    init_menu = false;

    // if (pressed) {
    //     Serial.print("Encoder press @: ");
    //     Serial.println(last_encoder_press);
    //     pressed = false;
    // }

    if (long_press) {
        Serial.println("Encoder long press");
        long_press = false;
    }

    delay(100);
}


void IRAM_ATTR shot_detected_ISR(void) {
    if (data.current_ammo > 0) {data.current_ammo--;}
    data.total_shots++;
}

void IRAM_ATTR btn0_ISR(void) {
    menus[current_menu]->btn1();
    return;

    current_menu++;
    if (current_menu >= NUMBER_OF_MENUS) {current_menu = 0;}
    init_menu = true;

    // menus[current_menu]->btn0();
}

void IRAM_ATTR encoder_1_ISR(void) {
    static unsigned long last_encoder_change = 0;
    unsigned long now = millis();

    // Debounce ISR
    if (now - last_encoder_change <= 100) {return;}
    
    // Since ISR activates on falling edge, if ENCODER_2 is 1 we ar turning CW, otherwise CCW
    if (digitalRead(ENCODER_2)) { // Turning clock wise
        if (inside_menu) {
            menus[current_menu]->scrollUp();
        } else {
            if (menu_to_clear == NULL) {menu_to_clear = menus[current_menu];}
            current_menu++;
            if (current_menu >= NUMBER_OF_MENUS) {current_menu = 0;}
            init_menu = true;
        }
    } else { // turning counter clock wise
        if (inside_menu) {
            menus[current_menu]->scrollUp();
        } else {
            if (menu_to_clear == NULL) {menu_to_clear = menus[current_menu];}
            current_menu--;
            if (current_menu >= NUMBER_OF_MENUS) {current_menu = NUMBER_OF_MENUS - 1;}
            init_menu = true;
        }
    }

    last_encoder_change = now;
}

void IRAM_ATTR encoder_key_event_ISR(void) {
    static unsigned long last_encoder_press = 0, last_event = 0;
    unsigned long now = millis();

    if (now - last_event < 50) {return;}

    if (digitalRead(ENCODER_KEY)) {
        if ((now - last_encoder_press) < LONG_PRESS_TIME_MS)    {inside_menu = menus[current_menu]->scrollKey();} 
        else                                                    {long_press = true;}
    
    } else {
        last_encoder_press = millis();
    }

    last_event = now;
}




