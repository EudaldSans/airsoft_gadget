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
}

void clear_screen() {
    tft.fillScreen(get_word_config(CFG_COLOR_BG));
}

TextBox::TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font){
    log_i("Created new textbox with text: %s, color: %04X and pos: (%d, %d)", new_text, new_color, pos_x, pos_y);
    
    this->x = pos_x;
    this->y = pos_y;
    this->text = new_text;
    this->color = new_color;
    this->font = font;
}

void TextBox::draw() {
    log_i("Drawing textbox");
    tft.loadFont(this->font);
    
    tft.setTextDatum(CC_DATUM);
    tft.setTextColor(this->color);
    tft.drawString(this->text, this->x, this->y);

    tft.unloadFont();
}

void TextBox::clear() {
    log_i("Clearing textbox");
    tft.loadFont(this->font);
    
    tft.setTextDatum(CC_DATUM);
    tft.setTextColor(get_word_config(CFG_COLOR_BG));
    tft.drawString(this->text, this->x, this->y);

    tft.unloadFont();
}

void TextBox::setColor(uint16_t new_color) {
    log_i("Text box color set to: %04X", new_color);
    this->color = new_color;
}

void TextBox::setText(String new_text) {
    log_i("Text box text set to: %s", new_text);
    this->clear();
    this->text = new_text;
}