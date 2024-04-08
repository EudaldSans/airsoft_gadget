// #include "menus.h"
// #include "config.h"

// #include "BlackOpsOne28.h"


// typedef enum {
//     ENTER_AMMO_SETTINGS = 0,
//     ENTER_CHRONO_SETTINGS,
//     ENTER_KDR_SETTINGS,
//     ENTER_COLOR_SETTINGS,
//     ENTER_TIME_SETTINGS,
//     LEAVE_SETTINGS_MENU,
//     MAX_MAIN_SETTINGS
// } main_menu_settings_t;

// String main_setting_names[] = {
//     "Ammo",
//     "Chrono",
//     "KDR",
//     "Color",
//     "Time",
//     "Exit"
// };

// typedef enum {
//     CONFIGURE_MAG_SIZE = 0,
//     CONFIGURE_RLD_BEHAVIOR,
//     LEAVE_AMMO_MENU,
//     MAX_AMMO_SETTINGS
// } ammo_menu_settings_t;

// String ammo_setting_names[] = {
//     "Mag size",
//     "Rld cfg",
//     "<<<"
// };

// typedef enum {
//     CONFIGURE_BB_WEIGHT = 0,
//     CONFIGURE_MAX_SPEED,
//     LEAVE_CHRONO_MENU,
//     MAX_CHRONO_SETTINGS
// } chromo_menu_settings_t;

// String chrono_setting_names[] = {
//     "BB weight",
//     "Max speed",
//     "<<<"
// };

// typedef enum {
//     LEAVE_KDR_MENU,
//     MAX_KDR_SETTINGS
// } kdr_menu_settings_t;

// String KDR_setting_names[] = {
//     "<<<"
// };

// typedef enum {
//     COLOR_100 = 0,
//     COLOR_50,
//     COLOR_25,
//     COLOR_BGND,
//     LEAVE_COLOR_MENU,
//     MAX_COLOR_SETTINGS
// } color_menu_settings_t;

// String color_setting_names[] = {
//     "100\% color",
//     "50\% color",
//     "25\% color",
//     "Bgnd color",
//     "<<<"
// };


// SettingsMenu::SettingsMenu(TFT_eSPI* p_tft):Menu(p_tft) {}

// void SettingsMenu::update(float heading, bool init) {  
//     if (init) {
//         this->_tft->fillScreen(get_word_config(CFG_COLOR_BG));
//         this->highlighted_setting = 0;
//     }

//     switch (this->current_menu) {
//         case MAIN_SETTINGS_MENU:    this->drawMenu(this->highlighted_setting, main_setting_names, MAX_MAIN_SETTINGS, init);         break;
//         case AMMO_SETTINGS:         this->drawMenu(this->highlighted_setting, ammo_setting_names, MAX_AMMO_SETTINGS, init);         break;
//         case CHRONO_SETTINGS:       this->drawMenu(this->highlighted_setting, chrono_setting_names, MAX_CHRONO_SETTINGS, init);     break;
//         case KDR_SETTINGS:          this->drawMenu(this->highlighted_setting, KDR_setting_names, MAX_KDR_SETTINGS, init);           break;
//         case COLOR_SETTINGS:        this->drawMenu(this->highlighted_setting, color_setting_names, MAX_COLOR_SETTINGS, init);       break;
//         default:                    log_e("Currently in unknown menu!");
//     }
// }

// // highlighted_setting is passed as argument on purpose, in order to avoid the scroll interrupt to chenge the value mid execution of the function
// void SettingsMenu::drawMenu(uint8_t highlighted_setting, String* setting_names, uint8_t max_settings, bool init) {
//     uint8_t current_seting_pos;

//     if (this->previous_setting == highlighted_setting && !init) {return;} 

//     this->_tft->loadFont(BlackOpsOne28);
//     this->_tft->setTextDatum(TC_DATUM);

//     if (this->menu_active) {
//         current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * previous_setting;
//         this->_tft->fillRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_BG));
//         current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * highlighted_setting;
//         this->_tft->fillRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_0));
//     }

//     for (int i = 0; i < max_settings; i++) {
//         current_seting_pos = 5 + MENU_SPACING + (this->small_font_height + MENU_SPACING) * i;

//         if (i == highlighted_setting && this->menu_active) {
//             this->_tft->setTextColor(get_word_config(CFG_COLOR_BG));
//             this->_tft->drawString(main_setting_names[i], SCREEN_CENTER, current_seting_pos);
            
//         } else {
//             this->_tft->setTextColor(get_word_config(CFG_COLOR_0));
//             this->_tft->drawString(main_setting_names[i], SCREEN_CENTER, current_seting_pos);
//             this->_tft->drawRect(5 + MENU_SPACING, current_seting_pos, SCREEN_WIDTH - 10 - MENU_SPACING * 2, this->small_font_height, get_word_config(CFG_COLOR_0));
//         }
//     }

//     this->previous_setting = highlighted_setting;
    
//     this->_tft->unloadFont();
// }



// void SettingsMenu::clear() {
//     this->_tft->fillScreen(get_word_config(CFG_COLOR_BG));
// }

// void SettingsMenu::up_button(unsigned long press_duration) {
//     this->highlighted_setting++;

//     switch (this->current_menu) {
//         case MAIN_SETTINGS_MENU:    if (this->highlighted_setting >= MAX_MAIN_SETTINGS) {this->highlighted_setting = 0;}        break;
//         case AMMO_SETTINGS:         if (this->highlighted_setting >= MAX_AMMO_SETTINGS) {this->highlighted_setting = 0;}        break;
//         case CHRONO_SETTINGS:       if (this->highlighted_setting >= MAX_CHRONO_SETTINGS) {this->highlighted_setting = 0;}      break;
//         case KDR_SETTINGS:          if (this->highlighted_setting >= MAX_KDR_SETTINGS) {this->highlighted_setting = 0;}         break;
//         case COLOR_SETTINGS:        if (this->highlighted_setting >= MAX_COLOR_SETTINGS) {this->highlighted_setting = 0;}       break;
//         default:                    log_e("Scrolled on an unknown settings menu!");
//     }
// }

// void SettingsMenu::down_button(unsigned long press_duration) {
//     this->highlighted_setting--;

//     switch (this->current_menu) {
//         case MAIN_SETTINGS_MENU:    if (this->highlighted_setting < 0) {this->highlighted_setting = MAX_MAIN_SETTINGS - 1;}     break;
//         case AMMO_SETTINGS:         if (this->highlighted_setting < 0) {this->highlighted_setting = MAX_AMMO_SETTINGS - 1;}     break;
//         case CHRONO_SETTINGS:       if (this->highlighted_setting < 0) {this->highlighted_setting = MAX_CHRONO_SETTINGS - 1;}   break;
//         case KDR_SETTINGS:          if (this->highlighted_setting < 0) {this->highlighted_setting = MAX_KDR_SETTINGS - 1;}      break;
//         case COLOR_SETTINGS:        if (this->highlighted_setting < 0) {this->highlighted_setting = MAX_COLOR_SETTINGS - 1;}    break;
//         default:                    log_e("Scrolled on an unknown settings menu!");
//     }
// }

// bool SettingsMenu::enter_button(unsigned long press_duration) {
//     this->previous_setting = 0xff;

//     if (!this->menu_active) {return Menu::enter_button(press_duration);}

//     if (this->current_menu == MAIN_SETTINGS_MENU && this->highlighted_setting == 5) {
//         return Menu::enter_button(press_duration);
//     } 

//     switch (this->current_menu) {
//         case MAIN_SETTINGS_MENU:    this->main_settings_key_press();      break;
//         case AMMO_SETTINGS:         this->ammo_settings_key_press();      break;
//         case CHRONO_SETTINGS:       this->chrono_settings_key_press();    break;
//         case KDR_SETTINGS:          this->kdr_settings_key_press();       break;
//         case COLOR_SETTINGS:        this->color_settings_key_press();     break;  
//         default:                    log_e("key press on an unknown settings menu!");
//     }   

//     return true;
// }



// void SettingsMenu::main_settings_key_press() {
//     switch (this->highlighted_setting) {
//         case ENTER_AMMO_SETTINGS:       this->current_menu = AMMO_SETTINGS;     break;
//         case ENTER_CHRONO_SETTINGS:     this->current_menu = CHRONO_SETTINGS;   break;
//         case ENTER_KDR_SETTINGS:        this->current_menu = KDR_SETTINGS;      break;
//         case ENTER_COLOR_SETTINGS:      this->current_menu = COLOR_SETTINGS;    break;
//         case ENTER_TIME_SETTINGS:       break;
//         default:                   log_e("Chose an unknown setting from main settings menu");       
//     }
// }

// void SettingsMenu::ammo_settings_key_press() {
//     if (this->highlighted_setting == LEAVE_AMMO_MENU) {this->current_menu = MAIN_SETTINGS_MENU;}
// }

// void SettingsMenu::chrono_settings_key_press() {
//     if (this->highlighted_setting == LEAVE_CHRONO_MENU) {this->current_menu = MAIN_SETTINGS_MENU;}
// }

// void SettingsMenu::kdr_settings_key_press() {
//     if (this->highlighted_setting == LEAVE_KDR_MENU) {this->current_menu = MAIN_SETTINGS_MENU;}
// }

// void SettingsMenu::color_settings_key_press() {
//     if (this->highlighted_setting == LEAVE_COLOR_MENU) {this->current_menu = MAIN_SETTINGS_MENU;}
// }
