#ifndef AMMO_SCREEN_H
#define AMMO_SCREEN_H

#include <Arduino.h>


void ammoScreenUpdate(float heading);
void ammoScreenInit(uint16_t configured_ammo);
void ammoScreenReload(void);

void ammoScreenDecreaseAmmo(void);

#endif