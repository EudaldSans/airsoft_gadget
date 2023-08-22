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
    uint16_t total_shots;
    uint16_t speed;  // TODO: maybe uint16_t is not the appropriate type, check later.
    uint16_t max_speed;
    float heading;
} screen_data_t;


typedef enum {
    AMMO_MENU = 0,
    KDR_MENU,
    CHRONO_MENU,
    NUMBER_OF_MENUS
} menu_t;


class Menu {
    public:
        Menu(TFT_eSPI* p_tft);
        virtual ~Menu(){};

        virtual void update(screen_data_t data, bool init);
        virtual void clear();

        virtual void btn0();
        virtual void btn1();
        virtual void btn2();

        virtual void scrollUp();
        virtual void scrollDown();
        virtual bool scrollKey();

        bool reload = false;
    
    protected:
        void updateHeading(float, uint16_t color, bool init);
        void updateArcMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init);
        void updateAmoCounter(uint16_t ammo, uint16_t color, bool init);
        void updateKDR(float kdr, uint16_t color, bool init);
        void updateCentralText(String str, uint16_t color, bool init);
        void updateMenuTitle(String str, uint16_t color, bool init);
        void display_menu_activity(uint16_t color, bool init);

        TFT_eSPI* _tft;
        uint16_t small_font_height, large_font_height;
        bool menu_active = false;
    
    private:
        String title = "ERR";
};

class AmmoMenu : public Menu {
    public:
        AmmoMenu(TFT_eSPI* p_tft);

        void update(screen_data_t data, bool init);

        bool reload = true;

        void scrollUp();
        void scrollDown();

    private:
        String title = "Ammo";
        uint8_t counting_mode;
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

class ChronoMenu : public Menu {
    public:
        ChronoMenu(TFT_eSPI* p_tft);

        void update(screen_data_t data, bool init);

        void scrollUp();
        void scrollDown();

        bool reload = false;

    private:
        void drawUnits(uint16_t color, bool init);
        void clear();
        
        uint16_t units;
        String title = "Chrono";
};
