#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <pico/stdlib.h>
#include <string.h>
#include "hardware/gpio.h"
#include "hardware/irq.h"

//bool reserved_addr(uint8_t);

// Function prototype for scan
void scan();


#ifdef __cplusplus
}
#endif


#endif 