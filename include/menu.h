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


typedef enum {
    DEC_AMMO_MENU = 0,
    INC_AMMO_MENU,
    NUMBER_OF_MENUS
} menu_t;

class Menu {
    public:
        Menu();
        virtual ~Menu(){};

        virtual void update(screen_data_t data, bool init);

        virtual void btn0();
        virtual void btn1();
        virtual void btn2();

        virtual void scrollUp();
        virtual void scrollDown();

        bool reload = false;
    
    protected:
        void updateHeading(float, uint16_t color, bool init);
        void updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init);
        void updateAmoCounter(uint16_t ammo, uint16_t color, bool init);

};

class DecreasingAmmoMenu : public Menu {
    public:
        DecreasingAmmoMenu();

        void update(screen_data_t data, bool init);

        bool reload = true;
};


class IncreasingAmmoMenu : public Menu {
    public:
        IncreasingAmmoMenu();

        void update(screen_data_t data, bool init);

        bool reload = true;
};