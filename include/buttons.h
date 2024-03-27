#pragma once

typedef void (*pButtonCallback_t) (unsigned long press_time);

void init_buttons(void);
void check_buttons(void);

void register_enter_cb (pButtonCallback_t cb);
void register_dual_cb (pButtonCallback_t cb);
void register_up_cb (pButtonCallback_t cb);
void register_down_cb (pButtonCallback_t cb);