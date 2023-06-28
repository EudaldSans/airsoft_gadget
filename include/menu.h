#include "Arduino.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#define SCREEN_CENTER       120
#define ARC_RADIOUS         120
#define ARC_START           30
#define ARC_END             330
#define ARC_THICKNESS       10

#define DARKER_GREY         0x18E3

typedef struct {
    uint16_t total_ammo;
    uint16_t current_ammo;
    uint16_t speed;  // TODO: maybe uint16_t is not the appropriate type, check later.
    float heading;
} screen_data_t;

class Menu {
    public:
        Menu();

        void update(screen_data_t data, bool init);

        void btn0();
        void btn1();
        void btn2();

        void scrollUp();
        void scrollDown();

        bool reload = false;
    
    protected:
        void updateHeading(float, uint16_t color, bool init);
        void updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init);
        void updateAmoCounter(uint16_t ammo, uint16_t color, bool init);

};

class DecreasingAmmoMenu : public Menu {
    public:
        DecreasingAmmoMenu();
        bool reload = true;
        void update(screen_data_t data, bool init);
};


class IncreasingAmmoMenu : public Menu {
    public:
        IncreasingAmmoMenu();
        bool reload = true;
        void update(screen_data_t data, bool init);
};