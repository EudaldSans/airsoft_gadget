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

#include "buttons.h"

#define UV_ENABLE       GPIO_NUM_14
#define IR_SENSOR_0     GPIO_NUM_16
#define IR_SENSOR_1     GPIO_NUM_34

#define PERIPHERAL_PSU  GPIO_NUM_5

#define FLASH_UPDATE_PERIOD_MS  500


TFT_eSPI tft = TFT_eSPI();

bool init_menu = true, inside_menu = false, go_to_sleep = false, cycle_uv = false;

HMC5883L mag;

Menu* menus[NUMBER_OF_MENUS];
Menu* menu_to_clear = NULL;
uint8_t current_menu;


void ir_sensr_0_ISR(void);
void ir_sensr_1_ISR(void);

void test_cb(unsigned long time) {
    Serial.print("Test cb with time: ");
    Serial.println(time);
}

/** 
 * @brief Setup function is called at the start of the program, sets up all peripherals and instantiates menus.
*/
void setup(void) {
    uint32_t splash_screen_color;
    Serial.begin(115200);
    Serial.println("Start");

    init_buttons();
    register_enter_cb(test_cb);
    return;

    // Setup all GPIOs
    pinMode(UV_ENABLE, OUTPUT);
    digitalWrite(PERIPHERAL_PSU, LOW);   // PSU is specially important since it powers all other components of the device

    pinMode(PERIPHERAL_PSU, OUTPUT);
    digitalWrite(PERIPHERAL_PSU, HIGH);

    // Wait for a bit for all peripherals to boot up, could be lower
    delay(25);

    // Initialize flash storage and recover color config
    init_config();
    splash_screen_color = get_word_config(CFG_COLOR_2);

    // Initialize LCD scrreen, set correct rotation
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(get_word_config(CFG_COLOR_BG));
    tft.drawXBitmap(SCREEN_CENTER - RTX_LOGO_W/2, SCREEN_CENTER - RTX_LOGO_H/2, RTX_logo_bitmap, RTX_LOGO_W, RTX_LOGO_H, splash_screen_color);
    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, 30, 50, splash_screen_color, TFT_BLACK);

    Wire.setPins(18, 13);
    // Join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // Initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // Verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
    
    // Prepare menus and set current menu to what is stored in flash
    menus[AMMO_MENU]        = new AmmoMenu(&tft);
    menus[KDR_MENU]         = new KDRMenu(&tft);
    menus[CHRONO_MENU]      = new ChronoMenu(&tft);
    menus[SETTINGS_MENU]    = new SettingsMenu(&tft);
    current_menu = get_config(CFG_CURRENT_MENU);

    // Attatch all interrupts
    attachInterrupt(IR_SENSOR_0, ir_sensr_0_ISR, FALLING);
    attachInterrupt(IR_SENSOR_1, ir_sensr_1_ISR, FALLING);

    // Draw a loading arc to make it look like we are doing some work while we show the logo
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
    
    // Clear the loading screen
    tft.fillScreen(get_word_config(CFG_COLOR_BG));
}

/**
 * @brief Main loop. Updates heading, updates current menu and clears previous one, updates flash values and makes MCU sleep when prompted
*/
void loop() {
    int16_t mx, my, mz;
    static unsigned long last_flash_update = 0;
    unsigned long now = millis();

    check_buttons();
    return;
    
    // Read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    heading = heading * 180/M_PI;

    if (menu_to_clear != NULL) {
        menu_to_clear->clear();
        menu_to_clear = NULL;
    }

    // Update flash values if a new menu is being displayed
    if (init_menu) {update_config(CFG_CURRENT_MENU, current_menu);}

    // Update current menu
    menus[current_menu]->update(heading, init_menu);
    init_menu = false;

    if (go_to_sleep) {
        Serial.println("Going to sleep. zZz zZz zZz...");

        save_all_configs();

        // Turn off all peripherals
        rtc_gpio_set_direction(PERIPHERAL_PSU, RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(PERIPHERAL_PSU, 0);

        // // Set enter button to wake up the device
        // rtc_gpio_pullup_en(ENTER_BUTTON);
        // esp_sleep_enable_ext0_wakeup(ENTER_BUTTON, 0);

        // // Go to sleep
        // esp_deep_sleep_start();
    }

    if ((now - last_flash_update) > FLASH_UPDATE_PERIOD_MS) {
       save_all_configs();
       last_flash_update = now; 
       Serial.println("Updated all flash values");
    }

    // There is no need to stress the MCU, ~10fps are more than enough.
    delay(100);
}

/**
 * @brief Interrupt trigger by IR sensor 1. Informa @c AmmoMenu that a shot has been dected.
*/
void IRAM_ATTR ir_sensr_0_ISR(void) {
    AmmoMenu* ammo_menu = static_cast<AmmoMenu*>(menus[AMMO_MENU]);
    if (ammo_menu != NULL) {ammo_menu->shot();}
}

/**
 * @brief Interrupt trigger by IR sensor 1. 
 * @todo Calculates speed using info from IR sensor 0 and updates chrono menu.
*/
void IRAM_ATTR ir_sensr_1_ISR(void) {
    
}


