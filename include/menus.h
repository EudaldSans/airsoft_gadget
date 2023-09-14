#pragma once

#include "Arduino.h"

#include <SPI.h>
#include <TFT_eSPI.h>
#include "config.h"

#define SCREEN_CENTER       120
#define SCREEN_WIDTH        240
#define SCREEN_HEIGHT       240

#define ARC_RADIOUS         120
#define ARC_START           30
#define ARC_END             330

#define MENU_SPACING        10

#define METER_THICKNESS     25
#define METER_WIDTH         230
#define METER_START         5
#define METER_Y             10

#define LONG_PRESS_TIME_MS      3000

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
    SETTINGS_MENU,
    NUMBER_OF_MENUS
} menu_t;

typedef enum {
    DECREASE_MODE = 0,
    INCREASE_MODE,
    TOTAL_MODE,
    NUMBER_OF_MODES
} counting_mode_t;

typedef enum {
    CHRONO_MPS = 0,
    CHRONO_FPS,
    NUMBER_OF_UNITS
} chrono_units_t;

typedef enum {
    MAIN_SETTINGS_MENU = 0,
    AMMO_SETTINGS,
    CHRONO_SETTINGS,
    KDR_SETTINGS,
    COLOR_SETTINGS,
    TIME_SETTINGS,
    NUMBER_OF_SETTING_MENUS
} setting_menus_t;


class Menu {
    public:
        Menu(TFT_eSPI* p_tft);
        virtual ~Menu(){};

        virtual void update(float heading, bool init);
        virtual void clear();

        virtual bool enter_button(unsigned long press_duration);
        virtual void up_button(unsigned long press_duration);
        virtual void down_button(unsigned long press_duration);
    
    protected:
        void updateHeading(float, uint16_t color, bool init);
        void updateMeter(uint16_t new_start_angle, uint16_t new_end_angle, uint16_t color, bool init);
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

        void update(float heading, bool init);

        void up_button(unsigned long press_duration);
        void down_button(unsigned long press_duration);
        void shot();

        void clear();

    private:
        String title = "Ammo";
        int8_t counting_mode;

        uint16_t current_ammo;
        uint16_t total_shots;

        void drawCountingMode(uint16_t color, bool init);
};

class KDRMenu : public Menu {
    public:
        KDRMenu(TFT_eSPI* p_tft);

        void update(float heading, bool init);

        void up_button(unsigned long press_duration);
        void down_button(unsigned long press_duration);


    private:
        uint16_t kills;
        uint16_t deaths;
        String title = "KDR";
};

class ChronoMenu : public Menu {
    public:
        ChronoMenu(TFT_eSPI* p_tft);

        void update(float heading, bool init);

        void up_button(unsigned long press_duration);

        void clear();

        uint16_t speed;

    private:
        void drawUnits(uint16_t color, bool init);
        
        int8_t units;
        String title = "Chrono";
};

class SettingsMenu : public Menu {
    public: 
        SettingsMenu(TFT_eSPI* p_tft);

        void update(float heading, bool init);

        bool enter_button(unsigned long press_duration);
        void up_button(unsigned long press_duration);
        void down_button(unsigned long press_duration);

        void clear();

    private:
        void drawSliderSetting(uint16_t min, uint16_t max, uint16_t pos);
        void drawColorSetting(uint32_t current_color);
        void drawStringSetting(bool draw_left, bool draw_right, String current_text);
        void drawYesNoSetting(bool highlighted_setting);

        void drawMenu(uint8_t highlighted_setting, String* setting_names, uint8_t max_settings, bool init);

        void main_settings_key_press();
        void ammo_settings_key_press();
        void chrono_settings_key_press();
        void kdr_settings_key_press();
        void color_settings_key_press();

        int8_t highlighted_setting = 0, previous_setting = 0xff;
        setting_menus_t current_menu = MAIN_SETTINGS_MENU;
        bool key_pressed = false, menu_up = false, menu_down = false;
        
};

// class Setting {
//     public:
//         Setting(String name, config_t setting_id, bool is_setting_word);
// };

// class SettingSubmenu {
//     public: 
//         SettingSubmenu(TFT_eSPI* p_tft, Setting* menu_settings);
// };