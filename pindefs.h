#ifndef PINDEFS_H
#define PINDEFS_H


#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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

#define MEM_BUF_SIZE 128
#define MEM_BUF_ROW_SIZE 9

//LED pins
#define LED_1_B 10
#define LED_1_R 11
#define LED_2_B 12
#define LED_2_R 13

//TOP pins
#define TOP_ENABLE 6
#define TOP_FAULT 7
#define TOP_CURRENT A1

//MEM_BUF ADDRESSES
#define ADD_ADDRESS 0
#define ADD_TYPE 1
#define ADD_NEIGHBOUR 2
#define ADD_POS_NEIGHBOUR 3
#define ADD_ACTIVE 7  // polarisation, switch state
#define ADD_LIVE_STATE 8 // pos_neighbour value if live, 0 if not, 1 for RED

//STATE MACHINE
#define STATE_SCAN 0
#define STATE_CHANGE 1
#define STATE_NO_CHANGE 2
#define STATE_CHANGE_LIVE 3


//DEVICE TYPES
#define BATT_TYPE 1
#define NODE_TYPE 2
#define WIRE_TYPE 3
#define MOTOR_TYPE 4
#define VAR_RESISTOR_TYPE 5
#define LED_TYPE 6
#define BUTTON_TYPE 7
#define BUZZER_TYPE 8
#define SENSOR_TYPE 9

//I2C ADDRESSES
#define I2C_ADD_ADDRESS 0
#define I2C_ADD_TYPE 4
#define I2C_ADD_NEIGHBOUR 8
#define I2C_ADD_POS_NEIGHBOUR 12
#define I2C_ADD_ACTIVE 16  // polarisation, switch state
#define I2C_ADD_LIVE_STATE 20 

#define ADD_BATTERY 1

// ERROR CODES
// 10 i2c_read
// 11 i2c_read
// 12 i2c_write
// 13 i2c_scan

#endif