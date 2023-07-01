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
    KDR_MENU,
    NUMBER_OF_MENUS
} menu_t;

class Menu {
    public:
        Menu(TFT_eSPI* p_tft);
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
        void updateKDR(float kdr, uint16_t color, bool init);
        void updateCentralText(String str, uint16_t color, bool init);
        void updateMenuTitle(String str, uint16_t color, bool init);

    private:
        TFT_eSPI* _tft;
        uint16_t small_font_height, large_font_height;
        String title = "ERR";
};

class DecreasingAmmoMenu : public Menu {
    public:
        DecreasingAmmoMenu(TFT_eSPI* p_tft);

        void update(screen_data_t data, bool init);

        bool reload = true;

    private:
        String title = "DecAmmo";
};


class IncreasingAmmoMenu : public Menu {
    public:
        IncreasingAmmoMenu(TFT_eSPI* p_tft);

        void update(screen_data_t data, bool init);

        bool reload = true;

    private:
        String title = "IncAmmo";
};


class KDRMenu : public Menu {
    public:
        KDRMenu(TFT_eSPI* p_tft);

        void update(screen_data_t data, bool init);

        void btn0();
        void btn1();
        void btn2();

        bool reload = false;

    private:
        uint16_t kills;
        uint16_t deaths;
        String title = "KDR";
};
