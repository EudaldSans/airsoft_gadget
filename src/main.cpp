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
#include "EG_logo.h"

#include "config.h"

#include <driver/rtc_io.h>

#define ENCODER_KEY     GPIO_NUM_15
#define ENCODER_1       GPIO_NUM_23
#define ENCODER_2       GPIO_NUM_19      

#define LONG_PRESS_TIME_MS      3000

#define FLASH_UPDATE_PERIOD_MS  500


TFT_eSPI tft = TFT_eSPI();

bool init_menu = true, inside_menu = false, long_press = false;

HMC5883L mag;

Menu* menus[NUMBER_OF_MENUS];
Menu* menu_to_clear = NULL;
uint8_t current_menu;


void shot_detected_ISR(void);
void btn0_ISR(void); 

void encoder_1_ISR(void);
void encoder_2_ISR(void);
void encoder_key_event_ISR(void);

void setup(void) {
    uint32_t splash_screen_color;
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    delay(25);

    init_config();
    splash_screen_color = get_word_config(CFG_COLOR_2);

    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(get_word_config(CFG_COLOR_BG));
    tft.drawXBitmap(SCREEN_CENTER - RTX_LOGO_W/2, SCREEN_CENTER - RTX_LOGO_H/2, RTX_logo_bitmap, RTX_LOGO_W, RTX_LOGO_H, splash_screen_color);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 30, 50, splash_screen_color, TFT_BLACK);

    Wire.setPins(22, 21);
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
    
    menus[AMMO_MENU]        = new AmmoMenu(&tft);
    menus[KDR_MENU]         = new KDRMenu(&tft);
    menus[CHRONO_MENU]      = new ChronoMenu(&tft);
    current_menu = get_config(CFG_CURRENT_MENU);

    pinMode(ENCODER_KEY, INPUT_PULLUP);
    pinMode(ENCODER_1, INPUT);
    pinMode(ENCODER_2, INPUT);

    // attachInterrupt(15, shot_detected_ISR, FALLING);
    attachInterrupt(0, btn0_ISR, FALLING);
    attachInterrupt(ENCODER_KEY, encoder_key_event_ISR, CHANGE);
    attachInterrupt(ENCODER_1, encoder_1_ISR, FALLING);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 50, 70, splash_screen_color, TFT_BLACK);

    delay(100);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 70, 90, splash_screen_color, TFT_BLACK);
    delay(150);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 90, 100, splash_screen_color, TFT_BLACK);
    delay(50);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 100, 150, splash_screen_color, TFT_BLACK);
    delay(250);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 150, 175, splash_screen_color, TFT_BLACK);
    delay(100);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 175, 225, splash_screen_color, TFT_BLACK);
    delay(300);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 225, ARC_END, splash_screen_color, TFT_BLACK);
    

    tft.fillScreen(get_word_config(CFG_COLOR_BG));
}


void loop() {
    int16_t mx, my, mz;
    static unsigned long last_flash_update = 0;
    unsigned long now = millis();
    
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    heading = heading * 180/M_PI;

    if (menu_to_clear != NULL) {
        menu_to_clear->clear();
        menu_to_clear = NULL;
    }

    if (init_menu) {
        update_config(CFG_CURRENT_MENU, current_menu);
    }

    menus[current_menu]->update(heading, init_menu);
    init_menu = false;

    if (long_press) {
        Serial.println("Encoder long press");
        long_press = false;
        tft.fillScreen(TFT_BLACK);
        rtc_gpio_set_direction(GPIO_NUM_4, RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(GPIO_NUM_4, 0);

        rtc_gpio_pullup_en(ENCODER_KEY);
        esp_sleep_enable_ext0_wakeup(ENCODER_KEY, 0);

        delay(100);
        esp_deep_sleep_start();
    }

    if ((now - last_flash_update) > FLASH_UPDATE_PERIOD_MS) {
       save_all_configs();
       last_flash_update = now; 
       Serial.println("Updated all flash values");
    }

    delay(100);
}


void IRAM_ATTR shot_detected_ISR(void) {
    AmmoMenu* ammo_menu = static_cast<AmmoMenu*>(menus[AMMO_MENU]);
    if (ammo_menu != NULL) {ammo_menu->shot();}
}

void IRAM_ATTR btn0_ISR(void) {
    menus[current_menu]->btn1();
    return;

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
            menus[current_menu]->scrollDown();
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




