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
    i2c_init(i2c, 100000);
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
            if (reserved_addr(addr))
                ret = PICO_ERROR_GENERIC;
            else
                ret = i2c_read_blocking(i2c, addr, &rxdata, 1, false);
            if (ret>0){
                printf("Found device at 0x%02x\n", addr);
            }
        }
        printf("Done.\n");
}


uint8_t i2c_read(i2c_inst_t *i2c, uint8_t addr, uint8_t internal_address) {
    uint8_t read_data = 0;

    // Send the internal address to the device
    int result = i2c_write_blocking(i2c, addr, &internal_address, 1, true);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C write failed\n");
        return;
    }

    // Read the data from the device
    result = i2c_read_blocking(i2c, addr, &read_data, 4, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C read failed\n");
        return;
    }

    // Print the data
    printf("Data read from address 0x%02X: 0x%02X\n", internal_address, read_data);

    return 0;
}


void i2c_write(i2c_inst_t *i2c, uint8_t addr, uint8_t internal_address, float data[2]) {

    // Send the internal address to the device
    int result = i2c_write_blocking(i2c, addr, &internal_address, 2, true);
    if (result == PICO_ERROR_GENERIC) {
        printf("I2C write failed\n");
        return;
    }
}