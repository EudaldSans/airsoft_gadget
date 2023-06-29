#include "menus.h"

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

#include "logo.h"

KDRMenu::KDRMenu(): Menu() {
    this->kills = 0;
    this->deaths = 0;
}

void KDRMenu::update(screen_data_t data, bool init) {
    float kdr;
    uint16_t meter_angle, color;
    String text;

    if (this->deaths == 0 && this-> kills == 0) {kdr = 1;}
    else if (this->deaths == 0) {kdr = this->kills;}
    else {kdr = (float)this->kills/this->deaths;}

    if (kdr >= 1) {color = TFT_GREEN;}
    else if (kdr >= 0.5) {color = TFT_YELLOW;}
    else {color = TFT_RED;}

    if (kdr > 100) {text = String((int)kdr, 10);}
    else if (kdr > 10|| (10 * kdr == (int)(10 * kdr))) {text = String(kdr, 1);}
    else {text = String(kdr, 2);}

    Menu::updateCentralText(text, color, init);

    if (kdr > 2) {kdr = 2;}

    meter_angle = 130 * kdr + 50;

    Menu::updateArcMeter(meter_angle - 20, meter_angle + 20, color, init);
    Menu::updateHeading(data.heading, color, init);
}

void KDRMenu::btn0() {};
void KDRMenu::btn1() {};
void KDRMenu::btn2() {};


