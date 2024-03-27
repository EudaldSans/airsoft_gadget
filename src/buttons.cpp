#include "Arduino.h"

#include "Wire.h"
#include <driver/rtc_io.h>

#include "buttons.h"

#define ENTER_BUTTON    GPIO_NUM_15
#define UP_BUTTON       GPIO_NUM_17
#define DOWN_BUTTON     GPIO_NUM_33

#define DEBOUNCE_TIME_MS            50
#define LONG_PRESS_TIME_MS          3000
#define CONSECUTIVE_ACTION_TIME_MS  500

unsigned long enter_press_time, enter_release_time;
unsigned long up_press_time, up_release_time;
unsigned long down_press_time, down_release_time;

bool enter_pressed, up_pressed, down_pressed;

pButtonCallback_t enter_cb = NULL, up_cb = NULL, down_cb = NULL, dual_cb = NULL;


void button_enter_ISR(void);
void button_up_ISR(void);
void button_down_ISR(void);


void init_buttons(void) {
    pinMode(ENTER_BUTTON, INPUT_PULLUP);
    pinMode(UP_BUTTON, INPUT_PULLUP);
    pinMode(DOWN_BUTTON, INPUT_PULLUP);

    attachInterrupt(ENTER_BUTTON, button_enter_ISR, CHANGE);
    attachInterrupt(UP_BUTTON, button_up_ISR, CHANGE);
    attachInterrupt(DOWN_BUTTON, button_down_ISR, CHANGE);

    Serial.println("Buttons initialized");
}


void register_enter_cb (pButtonCallback_t cb) {
    enter_cb = cb;
}

void register_dual_cb (pButtonCallback_t cb) {
    dual_cb = cb;
}

void register_up_cb (pButtonCallback_t cb) {
    up_cb = cb;
}

void register_down_cb (pButtonCallback_t cb) {
    down_cb = cb;
}


void check_buttons(void) {
    static unsigned long last_action_time = 0;
    if (!up_pressed && !down_pressed && !enter_pressed && (millis() - last_action_time) < CONSECUTIVE_ACTION_TIME_MS) {return;}

    // Buttons are tied to a pull up resistor, therefore are pressed when the value is 0
    if (enter_pressed || !digitalRead(ENTER_BUTTON)) {
        enter_pressed = false;

        if (!enter_cb) {Serial.println("enter CB not registered!");}
        else {enter_cb(millis() - enter_press_time);}
    }

    if ((up_pressed || !digitalRead(UP_BUTTON)) && (down_pressed || !digitalRead(DOWN_BUTTON))) {
        up_pressed = false;
        down_pressed = false;

        if (!down_cb) {Serial.println("dual CB not registered!");}
        else {down_cb(millis() - down_press_time);}

        goto end;
    }

    if (up_pressed || !digitalRead(UP_BUTTON)) {
        up_pressed = false;

        if (!up_cb) {Serial.println("up CB not registered!");}
        else {up_cb(millis() - down_press_time);}
    }

    if (down_pressed || !digitalRead(DOWN_BUTTON)) {
        down_pressed = false;

        if (!down_cb) {Serial.println("down CB not registered!");}
        else {down_cb(millis() - down_press_time);}
    }

end:
    last_action_time = millis();
}


/**
 * @brief Interrupt triggered by the button up. Sets press and release times of up button.
*/
void IRAM_ATTR button_up_ISR(void) {
    static unsigned long last_event = 0;
    unsigned long now = millis();

    if (now - last_event < DEBOUNCE_TIME_MS) {return;}
    last_event = now;

    if (!digitalRead(UP_BUTTON)) {up_press_time = now; up_pressed = true;}
    else {enter_release_time = now;}
}

/**
 * @brief Interrupt triggered by the button down. Sets press and release times of down button.
*/
void IRAM_ATTR button_down_ISR(void) {
    static unsigned long last_event = 0;
    unsigned long now = millis();

    if (now - last_event < DEBOUNCE_TIME_MS) {return;}
    last_event = now;

    if (!digitalRead(DOWN_BUTTON)) {down_press_time = now; down_pressed = true;}
    else {down_release_time = now;}
}

/**
 * @brief Interrupt triggered by the button enter. Sets press and release times of enter button.
*/
void IRAM_ATTR button_enter_ISR(void) {
    static unsigned long button_press_time = 0, last_event = 0;
    unsigned long now = millis();

    if (now - last_event < DEBOUNCE_TIME_MS) {return;}
    last_event = now;

    if (!digitalRead(ENTER_BUTTON)) {enter_press_time = now; enter_pressed = true;}
    else {enter_release_time = now;}
}