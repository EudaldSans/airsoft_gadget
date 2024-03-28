#include <TFT_eSPI.h>
#include <esp_log.h>

#include "config.h"
#include "screen.h"

#include "BlackOpsOne28.h"
#include "BlackOpsOne75.h"

#define MENU_SPACING        10

#define METER_THICKNESS     25
#define METER_WIDTH         230
#define METER_START         5
#define METER_Y             10


TFT_eSPI tft = TFT_eSPI();
uint16_t small_font_height, large_font_height;

void init_screen() {
    tft.begin();
    tft.setRotation(1);
    clear_screen();

    tft.loadFont(BlackOpsOne28);
    small_font_height = tft.fontHeight();
    tft.unloadFont(); 

    tft.loadFont(BlackOpsOne75);
    large_font_height = tft.fontHeight();
    tft.unloadFont(); 

    log_i("Large font height: %d", large_font_height);
    log_i("Small font height: %d", small_font_height);
}

void clear_screen() {
    tft.fillScreen(get_word_config(CFG_COLOR_BG));
}



TextBox::TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font){
    log_i("Created new TextBox with text: %s, color: %04X and pos: (%d, %d)", new_text, new_color, pos_x, pos_y);
    
    tft.loadFont(font);
    this->width = tft.textWidth(new_text);
    this->height = tft.fontHeight();
    if (this->width > SCREEN_WIDTH) {log_e("Text (%s) larger than SCREEN_WIDTH");}
    tft.unloadFont();
    
    this->x = pos_x;
    this->y = pos_y;
    this->text = new_text;
    this->color = new_color;
    this->font = font;
    this->is_visible = false;
}

void TextBox::draw(bool force) {
    if (this->is_visible && !force) {return;}
    log_i("Drawing TextBox %s", this->text);

    tft.loadFont(this->font);
    
    tft.setTextDatum(CC_DATUM);
    tft.setTextColor(this->color);
    tft.drawString(this->text, this->x, this->y);

    tft.unloadFont();

    this->is_visible = true;
}

void TextBox::clear() {
    if (!this->is_visible) {return;}
    log_i("Clearing TextBox %s", this->text);

    tft.loadFont(this->font);
    
    tft.setTextDatum(CC_DATUM);
    tft.setTextColor(get_word_config(CFG_COLOR_BG));
    tft.drawString(this->text, this->x, this->y);

    tft.unloadFont();

    this->is_visible = false;
}

void TextBox::setColor(uint16_t new_color) {
    log_i("TextBox (%s) color set to: %04X", this->text, new_color);
    this->color = new_color;

    if (this->is_visible){this->draw(true);}
}

void TextBox::setText(String new_text) {
    bool was_visible = this->is_visible;

    log_i("TextBox (%s) text set to: %s", this->text, new_text);
    if (this->is_visible) {
        this->clear();
    }
    this->text = new_text;

    if (was_visible) {this->draw(false);}
}

void TextBox::setPosition(int32_t pos_x, int32_t pos_y) {
    bool was_visible = this->is_visible;

    log_i("TextBox (%s) position set to: (%d, %d)", this->text, pos_x, pos_y);
    if (this->is_visible) {this->clear();}

    this->x = pos_x;
    this->y = pos_y;

    if (was_visible) {this->draw(false);}
}



Button::Button(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, uint16_t width, uint16_t height) {
    log_i("Created new Button with text: %s, color: %04X and pos: (%d, %d)", new_text, new_color, pos_x, pos_y);

    if (width > SCREEN_WIDTH) {log_e("Button larger than SCREEN_WIDTH");}
    if (height > SCREEN_HEIGHT) {log_e("Button larger than SCREEN_HEIGHT");}

    if (tft.textWidth(new_text) >= width) {log_e("Text (%s) larger than Button width %d", new_text, width);}
    if (small_font_height >= height) {log_e("Text (%s) larger than Button height %d", new_text, height);}

    this->x = pos_x;
    this->y = pos_y;
    this->text = new_text;
    this->color = new_color;
    this->is_visible = false;
    this->inverted = false;
    this->width = width;
    this->height = height;
};

void Button::draw(bool force) {
    if (this->is_visible && !force) {return;}
    log_i("Drawing Button %s", this->text);
    
    if (this->inverted) {
        tft.fillRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, this->color);

        tft.loadFont(BlackOpsOne28);
    
        tft.setTextDatum(CC_DATUM);
        tft.setTextColor(get_word_config(CFG_COLOR_BG));
        tft.drawString(this->text, this->x, this->y);

        tft.unloadFont();
    } else {
        tft.drawRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, this->color);

        tft.loadFont(BlackOpsOne28);
    
        tft.setTextDatum(CC_DATUM);
        tft.setTextColor(this->color);
        tft.drawString(this->text, this->x, this->y);

        tft.unloadFont();
    }

    this->is_visible = true;
};

void Button::invert() {
    log_i("Inverting Button %s", this->text);
    if (!this->is_visible) {this->inverted = !this->inverted; return;}

    // If button is inverted, call clear to erase all colored area.
    if (this->inverted) {
        this->clear();
    } 

    this->inverted = !this->inverted;
    this->draw(true);
};

void Button::clear() {
    if (!this->is_visible) {return;}
    log_i("Clearing Button %s", this->text);
    tft.fillRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, get_word_config(CFG_COLOR_BG));
    this->is_visible = false;
};

void Button::setText(String new_text) {
    bool was_visible = this->is_visible;

    log_i("Button (%s) text set to: %s", this->text, new_text);
    if (this->is_visible) {
        this->clear();
    }
    this->text = new_text;

    if (was_visible) {this->draw(false);}

};

void Button::setColor(uint16_t new_color) {
    log_i("Button (%s) color set to: %04X", this->text, new_color);
    this->color = new_color;

    if (this->is_visible){this->draw(true);}
};

void Button::setPosition(int32_t pos_x, int32_t pos_y) {
    bool was_visible = this->is_visible;

    log_i("Button (%s) position set to: (%d, %d)", this->text, pos_x, pos_y);
    if (this->is_visible) {this->clear();}

    this->x = pos_x;
    this->y = pos_y;

    if (was_visible) {this->draw(false);}
}; 

