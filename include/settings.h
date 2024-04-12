#pragma once

#include "Arduino.h"
#include "screen.h"
#include "config.h"

class Setting;

void settings_activate_setting (Setting* new_active_setting);
void settings_deactivate_setting ( void );
void settings_pressed_up(unsigned long press_time_ms);
void settings_pressed_down(unsigned long press_time_ms);
void settings_update_active_setting(bool force);


class Setting {
    public: 
        Setting(config_t config_id, String title);

        virtual void update(bool force);
        virtual void clear( void );

        virtual void pressed_up(unsigned long press_time_ms);
        virtual void pressed_center(unsigned long press_time_ms);
        virtual void pressed_down(unsigned long press_time_ms);

        void activate();
        void deactivate();
    
        TextBox* title;
        config_t config_id;

    private:
        bool active;
};


class BoolSetting : public Setting {
    public:
        BoolSetting(config_t config_id, String title);

        void update(bool force);
        void clear( void );

        void pressed_up(unsigned long time);
        void pressed_center(unsigned long time);
        void pressed_down(unsigned long time);

    private:
        bool state;
        Button* on_button;
        Button* off_button;
};

class SliderSetting : public Setting {
    public:
        SliderSetting(config_t config_id, String title, uint16_t max_level, uint16_t min_level);

        void update(bool force);
        void clear( void );

        void pressed_up(unsigned long time);
        void pressed_center(unsigned long time);
        void pressed_down(unsigned long time);
    private:
        Meter* setting_meter;
        TextBox* setting_value;
        uint16_t currrent_level, max_level, min_level;
        bool meter_updated;
};

class MultipleChoiceSetting : public Setting {
    public: 
        MultipleChoiceSetting(config_t config_id, String title);

        void update(bool force);
        void clear( void );

        void pressed_up(unsigned long time);
        void pressed_center(unsigned long time);
        void pressed_down(unsigned long time);
    private:
        uint8_t choice;
        Button* button_up, button_down, choice_0, choice_1, choice_2, choice_3;
};

