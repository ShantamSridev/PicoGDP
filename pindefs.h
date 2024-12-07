#ifndef PINDEFS_H
#define PINDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"



// I2C pins
#define SDA_PIN  16  // GPIO16 as SDA
#define SCL_PIN  17  // GPIO17 as SCL

#define LED_1_B 10
#define LED_1_R 11
#define LED_2_B 12
#define LED_2_R 13

#ifdef __cplusplus
}
#endif


#endif 