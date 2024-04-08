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

void draw_loading_bar(uint16_t progress, uint16_t color);

uint16_t get_small_text_width(String text);
uint16_t get_large_text_width(String text);
uint16_t get_small_text_height();
uint8_t get_large_text_height();

class Widget {
    public:
        Widget(int32_t pos_x, int32_t pos_y, uint16_t new_color);

        virtual void draw(bool force);
        virtual void clear();

        void setColor(uint16_t new_color);
        void setPosition(int32_t pos_x, int32_t pos_y);

        uint16_t getWidth( void );
        uint16_t getHeight( void );
        uint32_t getX( void );
        uint32_t getY( void );

    protected:
        uint16_t width, height;
        int32_t x, y;
        uint16_t color;
        bool is_visible;
};

class TextBox : public Widget {
    public:
        TextBox(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, const uint8_t* font);

        void draw(bool force);
        void clear();

        void setText(String new_text);


    protected:
        String text;
        const uint8_t* font;
};

class Button : public Widget {
    public: 
        Button(int32_t pos_x, int32_t pos_y, String new_text, uint16_t new_color, uint16_t width, uint16_t height);
        
        void draw(bool force);
        void invert();
        void clear();

        void setText(String new_text);
    
    private:
        String text;
        bool inverted;
};

class Meter : public Widget {
    public:
        Meter(int32_t pos_x, int32_t pos_y, int32_t pos_w, int32_t pos_h, uint16_t new_color, uint8_t start_level);

        void draw(bool force);
        void clear();

        void updateLevel(uint8_t new_level);
    
    private:
        uint8_t level;
};


class Image : public Widget {
    public: 
        Image(int32_t pos_x, int32_t pos_y, int16_t image_width, int16_t image_height, uint16_t new_color, const uint8_t* image_bmp);

        void draw(bool force);
        void clear();

    private:
        const uint8_t* image;
};
