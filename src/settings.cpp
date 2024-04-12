#include <esp_log.h>

#include "buttons.h"
#include "settings.h"

#include "BlackOpsOne28.h"

Setting* active_setting = NULL; 

void settings_activate_setting (Setting* new_active_setting) {
    if (active_setting != NULL) {active_setting->deactivate();}
    active_setting = new_active_setting;
    active_setting->activate();
}

void settings_deactivate_setting () {
    if (active_setting != NULL) {active_setting->deactivate();}
    active_setting = NULL;
}

void settings_pressed_up(unsigned long press_time_ms) {
    if (active_setting != NULL) {active_setting->pressed_up(press_time_ms);}
}

void settings_pressed_down(unsigned long press_time_ms) {
    if (active_setting != NULL) {active_setting->pressed_down(press_time_ms);}
}

void settings_update_active_setting (bool force) {
    if (active_setting != NULL) {active_setting->update(force);}
}


Setting::Setting(config_t config_id, String title) {
    this->active = false;
    this->config_id = config_id;

    this->title = new TextBox(SCREEN_CENTER, SCREEN_CENTER / 2, title, get_word_config(CFG_COLOR_0), BlackOpsOne28);
}

// FIXME: Should we call a callback?
void Setting::activate() {
    this->active = true;
}

// FIXME: Should we call a callback?
void Setting::deactivate() {
    this->active = false;
}

void Setting::pressed_up(unsigned long time)        {log_e("Pressed up on generic Setting");}
void Setting::pressed_center(unsigned long time)    {log_e("Pressed center on generic Setting");}
void Setting::pressed_down(unsigned long time)      {log_e("Pressed cown on generic Setting");}
void Setting::update(bool force)                    {log_e("Tried to update generic Setting");}
void Setting::clear()                               {log_e("Tried to clear generic Setting");}


/*******************/
/* BOOLEAN SETTING */
/*******************/

BoolSetting::BoolSetting(config_t config_id, String title) : Setting(config_id, title) {
    uint16_t button_width = screen_get_small_text_width("OFF") + 6;
    uint16_t button_height = screen_get_small_text_height() + 4;

    this->on_button = new Button(button_width + 20, SCREEN_CENTER + button_height, "ON", get_word_config(CFG_COLOR_0), button_width, button_height);
    this->off_button = new Button(SCREEN_WIDTH - (button_width + 20), SCREEN_CENTER  + button_height, "OFF", get_word_config(CFG_COLOR_0), button_width, button_height);

    this->state = (get_config(config_id) != 0);

    if (this->state) {
        this->on_button->select();
    } else {
        this->off_button->select();
    }
}

void BoolSetting::update(bool force) {   
    log_i("Updating BoolSetting with id %d, force=%d", this->config_id, force);
    this->title->draw(force);
    
    if (this->state) {
        this->on_button->select();
        this->off_button->desselect();
    } else {
        this->off_button->select();
        this->on_button->desselect();
    }

    this->on_button->draw(false);
    this->off_button->draw(false);
}

void BoolSetting::clear( void ) {
    log_i("Clearing BoolSetting with id %d", this->config_id);
    this->title->clear();
    this->on_button->clear();
    this->off_button->clear();
}

void BoolSetting::pressed_up(unsigned long press_time_ms) {
    if (press_time_ms == 0 || press_time_ms >= LONG_PRESS_TIME_MS) {this->state = !this->state;}
}

void BoolSetting::pressed_down(unsigned long press_time_ms) {
    if (press_time_ms == 0 || press_time_ms >= LONG_PRESS_TIME_MS) {this->state = !this->state;}
}

// FIXME: probably shoul dbe calling callbacks to tell the parent when it's finished?
void BoolSetting::pressed_center(unsigned long press_time_ms) {
    uint8_t config_write_value = this->state ? 0x01 : 0x00;

    update_config(this->config_id, config_write_value);
    this->deactivate();
}


/******************/
/* SLIDER SETTING */
/******************/

SliderSetting::SliderSetting(config_t config_id, String title, uint16_t max_level, uint16_t min_level) : Setting(config_id, title) {
    this->currrent_level = get_word_config(config_id);
    if (this->currrent_level > max_level) {this->currrent_level = max_level;}
    if (this->currrent_level < min_level) {this->currrent_level = min_level;}

    uint8_t meter_height = 20;
    uint8_t text_box_height = screen_get_small_text_height();
    uint16_t meter_percentage = this->currrent_level * 100 / this->max_level;

    this->setting_meter = new Meter(SCREEN_CENTER, SCREEN_CENTER + meter_height / 2, SCREEN_WIDTH - 10, meter_height, get_word_config(CFG_COLOR_0), meter_percentage);
    this->setting_value = new TextBox(SCREEN_CENTER, SCREEN_CENTER + meter_height + text_box_height/2, String(this->currrent_level), get_word_config(CFG_COLOR_0), BlackOpsOne28);

    this->max_level = max_level;
    this->min_level = min_level;

    this->meter_updated = false;
}

void SliderSetting::update(bool force) {
    static uint16_t previous_level = 0;
    uint16_t new_percentage = this->currrent_level * 100 / this->max_level;
    log_i("Updating SliderSetting with id %d, force=%d", this->config_id, force);

    this->title->draw(force);

    if (previous_level != this->currrent_level) {
        this->setting_meter->setLevel(new_percentage);
        this->setting_value->setText(String(this->currrent_level));
        previous_level = this->currrent_level;
    } else {
        this->setting_meter->draw(force);
        this->setting_value->draw(force);
    }
}

void SliderSetting::clear( void ) {
    log_i("Clearing SliderSetting with id %d", this->config_id);
    this->title->clear();
    this->setting_meter->clear();
}

void SliderSetting::pressed_up(unsigned long press_time_ms) {
    if (this->currrent_level >= this->max_level) {return;}
    if (press_time_ms == 0) {this->currrent_level++;}
    
    if (press_time_ms >= LONG_PRESS_TIME_MS) {this->currrent_level++;}
}

void SliderSetting::pressed_down(unsigned long press_time_ms) {
    if (this->currrent_level <= this->min_level) {return;}
    if (press_time_ms == 0) {this->currrent_level--;}
    
    if (press_time_ms >= LONG_PRESS_TIME_MS) {this->currrent_level--;}
}

// FIXME: probably should be calling callbacks to tell the parent when it's finished?
void SliderSetting::pressed_center(unsigned long press_time_ms) {
    update_word_config(this->config_id, this->currrent_level);
    this->deactivate();
}

