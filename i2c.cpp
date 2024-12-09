#include "i2c.h"


void i2c_start_init(i2c_inst_t *i2c){

    // Set up the I2C pins
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Configure GPIO16 as SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Configure GPIO17 as SCL
    gpio_pull_up(SDA_PIN); // Enable pull-up resistor for SDA
    gpio_pull_up(SCL_PIN); // Enable pull-up resistor for SCL
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    // Initialize I2C with a baud rate of 100 kHz
    i2c_init(i2c, BAUD_RATE);
}

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void i2c_scan(i2c_inst_t *i2c){
        for (int addr = 0; addr < (1 << 7); ++addr) {

            // Skip over any reserved addresses.
            int ret=0;
            uint8_t rxdata;
            //printf("Scanning address 0x%02x\n", addr);
            if (reserved_addr(addr)){
                ret = PICO_ERROR_GENERIC;
            }
            else
                ret = i2c_read_blocking(i2c, addr, &rxdata, 1, false);
            if (ret>0){
                printf("Found device at 0x%02x\n", addr);
                
            }
        }
        printf("Done.\n");
}

void i2c_read(i2c_inst_t *i2c, uint8_t addr, uint8_t internal_address, uint8_t *buf, size_t length) {

    // Send the internal address to the device
    int result = i2c_write_blocking(i2c, addr, &internal_address, 1, true);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C READ_Write failed - CODE 10\n");
        return;
    }


    result = i2c_read_blocking(i2c, addr, buf, length, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C READ failed - CODE 11\n");
    }


    //  uint8_t buf[4];
    //     i2c_read(i2c, 0x09, 120, buf, READLENGTH); 

    //     printf("Received: ");
    //     for (int i = 0; i < 4; i++) {
    //         printf("%d ", buf[i]);
    //     }
    //     printf("\n");
}

void i2c_write(i2c_inst_t *i2c, uint8_t addr, uint8_t internal_address, uint8_t data, size_t length) {
    uint8_t buf[2] = {internal_address, data};
    int result = i2c_write_blocking(i2c, addr, buf, length, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C WRITE failed - CODE 12\n");
    }

    //i2c_write(i2c, 0x09, 10, 50, WRITELENGTH);
}