#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pindefs.h"

void led_init();

void led_on(uint8_t led);

void led_off(uint8_t led);

#ifdef __cplusplus
}
#endif
#endif 



