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
#include "pindefs.h"



void i2c_start_init(i2c_inst_t *);

bool reserved_addr(uint8_t);

void i2c_scan(i2c_inst_t *);

uint8_t i2c_read(i2c_inst_t *, uint8_t, uint8_t);

void i2c_write(i2c_inst_t *, uint8_t, uint8_t, float[4]);

#ifdef __cplusplus
}
#endif
#endif 