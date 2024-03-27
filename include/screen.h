#pragma once

#include "Arduino.h"

#define SCREEN_CENTER       120
#define SCREEN_WIDTH        240
#define SCREEN_HEIGHT       240

#define ARC_RADIOUS         120
#define ARC_START           30
#define ARC_END             330


void init_screen();
void clear_screen();

class TextBox {
    public:
        TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font);

        void draw();

        void setText(String new_text);
        void setColor(uint16_t new_color);

        void clear();

    protected:
        int32_t x, y;
        uint16_t color;
        String text;
        const uint8_t* font;
};

class Button {
    public: 
        Button(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color);
        
        void draw();
        void invert();
        void clear();
};

class Meter {
    public:
        Meter(int32_t pos_x, int32_t pos_y, int32_t pos_w, int32_t pos_h, uint16_t new_color, uint8_t new_level);

        void draw();
        void updateLevel(uint8_t new_level);
        void updateColor(uint16_t new_color);
    
    protected:
        int32_t x, y, w, h;
        uint16_t color;
        uint8_t level;
};


// TODO: class Image

