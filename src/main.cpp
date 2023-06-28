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

#include "menu.h"

uint16_t total_ammo = 30, current_ammo = 30;

HMC5883L mag;
int16_t mx, my, mz;

DecreasingAmmoMenu* dec_ammo_menu;
IncreasingAmmoMenu* inc_ammo_menu;

void shot_detected_ISR(void);

void setup(void) {
    screen_data_t data = {0};
    data.current_ammo = current_ammo;
    data.total_ammo = total_ammo;
    data.heading = 0;

    Serial.begin(115200);
    Serial.println("Start");

    // ammoScreenInit(30);

    Wire.setPins(22, 21);
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize device
    Serial.println("Initializing I2C devices...");
    mag.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");

    Serial.println("Starting DecreasingAmmoMenu");
    
    // dec_ammo_menu = new DecreasingAmmoMenu;
    // dec_ammo_menu->update(data, true);

    inc_ammo_menu = new IncreasingAmmoMenu;
    inc_ammo_menu->update(data, true);

    attachInterrupt(15, shot_detected_ISR, FALLING);
    // attachInterrupt(0, ammoScreenReload, FALLING);
}


void loop() {
    screen_data_t data = {0};
    data.current_ammo = current_ammo;
    data.total_ammo = total_ammo;
    // read raw heading measurements from device
    mag.getHeading(&mx, &my, &mz);
    
    // To calculate heading in degrees. 0 degree indicates North
    float heading = atan2(my, mx);
    if(heading < 0) heading += 2 * M_PI;
    heading = heading * 180/M_PI;

    data.heading = heading;

    // dec_ammo_menu->update(data, false);
    inc_ammo_menu->update(data, false);

    delay(200);
}


void IRAM_ATTR shot_detected_ISR(void) {
    if (current_ammo > 0) {current_ammo--;}
}



