#include "menus.h"
#include "config.h"

#include "BlackOpsOne75.h"
#include "BlackOpsOne28.h"

KDRMenu::KDRMenu(TFT_eSPI* p_tft): Menu(p_tft) {
    this->kills = 0;
    this->deaths = 0;
}

void KDRMenu::update(float heading, bool init) {
    float kdr;
    uint16_t meter_angle, color;
    String text;

    if (this->deaths == 0 && this-> kills == 0) {kdr = 1;}
    else if (this->deaths == 0) {kdr = this->kills;}
    else {kdr = (float)this->kills/this->deaths;}

    if (kdr >= 1) {color = get_word_config(CFG_COLOR_0);}
    else if (kdr >= 0.5) {color = get_word_config(CFG_COLOR_1);}
    else {color = get_word_config(CFG_COLOR_2);}

    if (kdr > 100 || (kdr == (int)kdr)) {text = String((int)kdr, 10);}
    else if (kdr > 10|| (10 * kdr == (int)(10 * kdr))) {text = String(kdr, 1);}
    else {text = String(kdr, 2);}

    Menu::updateCentralText(text, color, init);

    if (kdr > 2) {kdr = 2;}

    meter_angle = 130 * kdr + 50;

    Menu::updateArcMeter(meter_angle - 20, meter_angle + 20, color, init);
    Menu::updateHeading(heading, color, init);
    Menu::updateMenuTitle(this->title, color, init);
    Menu::display_menu_activity(color, init);
}

void KDRMenu::btn0() {
    this->kills++;
};

void KDRMenu::btn1() {};
void KDRMenu::btn2() {};


