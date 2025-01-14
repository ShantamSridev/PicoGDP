#ifndef LED_H
#define LED_H


#include "pindefs.h"

void led_init();

void led_on(uint8_t led);

void led_off(uint8_t led);

void all_leds_on();

void all_leds_off();

void pwm_white(uint8_t led);


#endif 



