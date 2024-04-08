#pragma once

#include "Arduino.h"
#include "screen.h"
#include "config.h"


class Setting {
    public: 
        Setting(config_t config_id);

        virtual void update(bool force);
        virtual void clear( void );

        virtual void pressed_up(unsigned long time);
        virtual void pressed_center(unsigned long time);
        virtual void pressed_down(unsigned long time);

        void activate();
        void deactivate();

    private:
        bool active;
        TextBox title;
        config_t config_id;
};


class BoolSetting : public Setting {
    public:
        BoolSetting(config_t config_id);

    private:
        Button on_button;
        Button off_button;
};

class SliderSetting : public Setting {
    public:
        SliderSetting(config_t config_id);

    private:
        Meter setting_meter;
        uint16_t currrent_level;
};

class MultipleChoiceSetting : public Setting {
    public: 
        MultipleChoiceSetting(config_t config_id);

    private:
        Button button_up, button_down, choice_0, choice_1, choice_2, choice_3;
};

