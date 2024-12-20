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


// I2C
#define SDA_PIN  16  // GPIO16 as SDA
#define SCL_PIN  17  // GPIO17 as SCL

#define I2CINSTANCE i2c0
#define BAUD_RATE 100000
#define WRITELENGTH 2
#define READLENGTH 4

#define TESTSCANLENGTH 10 // for testing purposes for workshop

#define MEM_BUF_SIZE 128
#define MEM_BUF_ROW_SIZE 7
#define MEM_BUF_NEIGHBOUR_START 3

//LED pins
#define LED_1_B 10
#define LED_1_R 11
#define LED_2_B 12
#define LED_2_R 13

//TOP pins
#define TOP_ENABLE 6
#define TOP_FAULT 7
#define TOP_CURRENT A1


#define ADD_TYPE 0
#define ADD_NIEGHBOURS 4
#define ADD_POS_NEIGHBOUR 8
#define ADD_POLARISATION 12
#define ADD_LIVE_STATE 16

#define STATE_SCAN 0
#define STATE_CHANGE 2
#define STATE_NO_CHANGE 3
#define STATE_NO_CHANGE_LIVE 4

// ERROR CODES
// 10 i2c_read
// 11 i2c_read
// 12 i2c_write
// 13 i2c_scan

#define BATT_TYPE 1
#define NODE_TYPE 2
#define WIRE_TYPE 3
#define BUTTON_TYPE 4
#define VAR_RESISTOR_TYPE 5
#define LED_TYPE 6
#define MOTOR_TYPE 7
#define BUZZER_TYPE 8
#define SENSOR_TYPE 9

#ifdef __cplusplus
}


#endif
#endif