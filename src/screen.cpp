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

void draw_loading_bar(uint16_t progress, uint16_t color) {
    static uint16_t previous_progress = 30;

    if (progress < previous_progress) {return;}
    if (progress > ARC_END) {progress = ARC_END;} // Is this OK? Test it!

    tft.drawArc(SCREEN_CENTER, SCREEN_CENTER, ARC_RADIOUS, ARC_RADIOUS - 10, previous_progress, progress, color, TFT_BLACK);
    previous_progress = progress;
}


Widget::Widget(int32_t pos_x, int32_t pos_y, uint16_t new_color) {
    this->x = pos_x;
    this->y = pos_y;

    this->color = new_color;
    this->is_visible = false;
}

void Widget::draw(bool force) {log_e("Called generic Widget draw");}
void Widget::clear() {log_e("Called generic Widget clear");}

void Widget::setColor(uint16_t new_color) {
    log_i("Widget color set to: %04X", new_color);
    this->color = new_color;

    if (this->is_visible){this->draw(true);}
}

void Widget::setPosition(int32_t pos_x, int32_t pos_y) {
    bool was_visible = this->is_visible;

    log_i("Widget position set to: (%d, %d)", pos_x, pos_y);
    if (this->is_visible) {this->clear();}

    this->x = pos_x;
    this->y = pos_y;

    if (was_visible) {this->draw(false);}
}

uint16_t Widget::getWidth( void ) {return this->width;;}
uint16_t Widget::getHeight( void ) {return this->height;}
uint32_t Widget::getX( void ) {return this->x;}
uint32_t Widget::getY( void ) {return this->y;}


TextBox::TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font) : Widget(pos_x, pos_y, new_color){
    log_i("Created new TextBox with text: %s, color: %04X and pos: (%d, %d)", new_text, new_color, pos_x, pos_y);
    
    tft.loadFont(font);
    this->width = tft.textWidth(new_text);
    this->height = tft.fontHeight();
    if (this->width > SCREEN_WIDTH) {log_e("Text (%s) larger than SCREEN_WIDTH");}
    tft.unloadFont();
    
    this->text = new_text;
    this->font = font;
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

void TextBox::setText(String new_text) {
    bool was_visible = this->is_visible;

    log_i("TextBox (%s) text set to: %s", this->text, new_text);
    if (this->is_visible) {
        this->clear();
    }
    this->text = new_text;

    if (was_visible) {this->draw(false);}
}



Button::Button(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, uint16_t width, uint16_t height) : Widget(pos_x, pos_y, new_color) {
    log_i("Created new Button with text: %s, color: %04X and pos: (%d, %d)", new_text, new_color, pos_x, pos_y);

    if (width > SCREEN_WIDTH) {log_e("Button larger than SCREEN_WIDTH");}
    if (height > SCREEN_HEIGHT) {log_e("Button larger than SCREEN_HEIGHT");}

    if (tft.textWidth(new_text) >= width) {log_e("Text (%s) larger than Button width %d", new_text, width);}
    if (small_font_height >= height) {log_e("Text (%s) larger than Button height %d", new_text, height);}

    this->text = new_text;
    this->inverted = false;
    this->width = width;
    this->height = height;
}

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
}

void Button::invert() {
    log_i("Inverting Button %s", this->text);
    if (!this->is_visible) {this->inverted = !this->inverted; return;}

    // If button is inverted, call clear to erase all colored area.
    if (this->inverted) {
        this->clear();
    } 

    this->inverted = !this->inverted;
    this->draw(true);
}

void Button::clear() {
    if (!this->is_visible) {return;}
    log_i("Clearing Button %s", this->text);
    tft.fillRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, get_word_config(CFG_COLOR_BG));
    this->is_visible = false;
}

void Button::setText(String new_text) {
    bool was_visible = this->is_visible;

    log_i("Button (%s) text set to: %s", this->text, new_text);
    if (this->is_visible) {
        this->clear();
    }
    this->text = new_text;

    if (was_visible) {this->draw(false);}

}


Meter::Meter(int32_t pos_x, int32_t pos_y, int32_t width, int32_t height, uint16_t new_color, uint8_t start_level) : Widget(pos_x, pos_y, new_color) {
    if (width > SCREEN_WIDTH) {log_e("Meter larger than SCREEN_WIDTH");}
    if (height > SCREEN_HEIGHT) {log_e("Meter larger than SCREEN_HEIGHT");}

    this->width = width;
    this->height = height;
    this->level = start_level;
}

void Meter::draw(bool force) {
    if (this->is_visible && !force) {return;}
    log_i("Drawing Meter");

    uint8_t meter_fill = (this->width - 4) * this->level / 100;

    tft.drawRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, this->color);
    tft.fillRect(this->x - this->width/2 + 2, this->y - this->height/2 + 2, meter_fill, this->height - 4, this->color);

    this->is_visible = true;
}

void Meter::clear() {
    if (!this->is_visible) {return;}
    log_i("Drawing Meter");

    uint8_t bar_width = (this->width - 4) * this->level / 100;

    tft.fillRect(this->x - this->width/2, this->y - this->height/2, this->width, this->height, get_word_config(CFG_COLOR_BG));
    // tft.fillRect(this->x - this->width/2 + 2, this->y - this->height/2 + 2, bar_width, this->height - 4, get_word_config(CFG_COLOR_BG));

    this->is_visible = false;
}

void Meter::updateLevel(uint8_t new_level) {
    if (!this->is_visible) {this->level = new_level; return;}

    uint8_t previous_bar_width = (this->width - 4) * this->level / 100;
    uint8_t new_bar_width = (this->width - 4) * this->level / 100;

    if (previous_bar_width == new_bar_width) {return;}
    else if (previous_bar_width > new_bar_width) {
        tft.fillRect(this->x - this->width/2 + 2 + new_bar_width, this->y - this->height/2 + 2, previous_bar_width - new_bar_width, this->height - 4, get_word_config(CFG_COLOR_BG));
    } else {
        tft.fillRect(this->x - this->width/2 + 2 + previous_bar_width, this->y - this->height/2 + 2, new_bar_width - previous_bar_width, this->height - 4, this->color);
    }
}

// tft.drawXBitmap(SCREEN_CENTER - RTX_LOGO_W/2, SCREEN_CENTER - RTX_LOGO_H/2, RTX_logo_bitmap, RTX_LOGO_W, RTX_LOGO_H, splash_screen_color);

Image::Image(int32_t pos_x, int32_t pos_y, int16_t image_width, int16_t image_height, uint16_t new_color, const uint8_t* image_bmp) : Widget(pos_x, pos_y, new_color) {
    this->x = pos_x;
    this->y = pos_y;
    this->width = image_width;
    this->height = image_height;
    this->image = image_bmp;
    this->color = new_color;
}

void Image::draw(bool force) {
    if (this->is_visible && !force) {return;}
    log_i("Drawing Image");

    tft.drawXBitmap(this->x, this->y, this->image, this->width, this->height, this->color);
}

void Image::clear() {
    if (!this->is_visible) {return;}
    log_i("Clearing Image");

    tft.drawXBitmap(this->x, this->y, this->image, this->width, this->height, get_word_config(CFG_COLOR_BG));
}
