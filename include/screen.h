#pragma once

#include "Arduino.h"

#define SCREEN_CENTER       120
#define SCREEN_WIDTH        240
#define SCREEN_HEIGHT       240

#define ARC_RADIOUS         120
#define ARC_START           30
#define ARC_END             330

#define LARGE_FONT_HEIGHT   68
#define SMALL_FONT_HEIGHT   27


void init_screen();
void clear_screen();

class TextBox {
    public:
        TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font);

        void draw(bool force);

        void setText(String new_text);
        void setColor(uint16_t new_color);
        void setPosition(int32_t pos_x, int32_t pos_y);

        void clear();

        uint16_t width, height;

    protected:
        int32_t x, y;
        uint16_t color;
        String text;
        const uint8_t* font;
        bool is_visible;
};

class Button {
    public: 
        Button(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, uint16_t width, uint16_t height);
        
        void draw(bool force);
        void invert();
        void clear();

        void setText(String new_text);
        void setColor(uint16_t new_color);
        void setPosition(int32_t pos_x, int32_t pos_y); 

        uint16_t width, height;
    
    private:
        int32_t x, y;
        uint16_t color;
        String text;
        bool is_visible, inverted;
};

class Meter {
    public:
        Meter(int32_t pos_x, int32_t pos_y, int32_t pos_w, int32_t pos_h, uint16_t new_color, uint8_t new_level);

        void draw(bool force);
        void updateLevel(uint8_t new_level);
        void updateColor(uint16_t new_color);

        uint16_t width, height;
    
    protected:
        int32_t x, y, w, h;
        uint16_t color;
        uint8_t level;
};


// TODO: class Image

