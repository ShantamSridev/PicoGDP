// I2C Bus Scan
//    0 1 2 3 4 5 6 7 8 9 A B C D E F
// 00 . . . . . . . . . . . . . . . .
// 10 . . @ . . . . . . . . . . . . .
// 20 . . . . . . . . . . . . . . . .
// 30 . . . . @ . . . . . . . . . . .
// 40 . . . . . . . . . . . . . . . .
// 50 . . . . . . . . . . . . . . . .
// 60 . . . . . . . . . . . . . . . .
// 70 . . . . . . . . . . . . . . . .
// E.g. if addresses 0x12 and 0x34 were acknowledged.

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Enable UART so we can print status output
    
    stdio_init_all();
    sleep_ms(10000);
    printf("I2C Bus Scan Initialised\n");
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    while(1){
        printf("\nI2C Bus Scan\n");
        printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

        for (int addr = 0; addr < (1 << 7); ++addr) {
            if (addr % 16 == 0) {
                printf("%02x ", addr);
            }

            // Perform a 1-byte dummy read from the probe address. If a slave
            // acknowledges this address, the function returns the number of bytes
            // transferred. If the address byte is ignored, the function returns
            // -1.

            // Skip over any reserved addresses.
            int ret=0;
            uint8_t rxdata;
            if (reserved_addr(addr))
                ret = PICO_ERROR_GENERIC;
            else
                ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

            printf(ret < 0 ? "." : "@");
            printf(addr % 16 == 15 ? "\n" : "  ");
        }
        printf("Done.\n");
        sleep_ms(2000);
    }
    return 0;
#endif
}


// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "pico/binary_info.h"
// #include "hardware/i2c.h"

// #define I2C_PORT i2c0               // Use I2C port 0
// #define I2C_SLAVE1_ADDR 0x08         // Slave address (same as in the slave code)
// #define I2C_SLAVE2_ADDR 0x09         // Slave address (same as in the slave code)
// #define SDA_PIN 4                  // GPIO pin for SDA
// #define SCL_PIN 5                   // GPIO pin for SCL
// #define LED_PIN PICO_DEFAULT_LED_PIN  // On-board LED pin

// int main() {
//     // Initialize chosen serial port for debugging
//     stdio_init_all();

//     // Initialize I2C port at 100 kHz
//     i2c_init(I2C_PORT, 100 * 1000);

//     // Set up I2C pins with pull-up resistors
//     gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
//     gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
//     // gpio_pull_up(SDA_PIN);
//     // gpio_pull_up(SCL_PIN);
//     sleep_ms(10000);
//     // Set up the LED pin
//     gpio_init(LED_PIN);
//     gpio_set_dir(LED_PIN, GPIO_OUT);

//     // Give some time for initialization
//     sleep_ms(1000);
//     printf("I2C Master Started\n");

//     while (1) {
//         // toggle the LED to indicate communication
//         gpio_put(LED_PIN, !gpio_get(LED_PIN));
//         printf("Toggling LED\n");

//         // Send data to the slave
//         uint8_t data_to_send[16] = "Hello Slave!";
//         int ret = i2c_write_blocking(I2C_PORT, I2C_SLAVE1_ADDR, data_to_send, sizeof(data_to_send) - 1, false);
//         if (ret < 0) {
//             printf("Failed to write to slave\n");
//         } else {
//             printf("Sent data to slave: %s\n", data_to_send);
//         }

//         // Send data to the slave
//         uint8_t data_to_send2[] = "Hello Slave2!";
//         ret = i2c_write_blocking(I2C_PORT, I2C_SLAVE2_ADDR, data_to_send2, sizeof(data_to_send) - 1, false);
//         if (ret < 0) {
//             printf("Failed to write to slave\n");
//         } else {
//             printf("Sent data to slave: %s\n", data_to_send);
//         }

//         // Wait for the slave to process the data
//         sleep_ms(100);

//         // Buffer to store received data
//         uint8_t rxdata[32];

//         // Read data from the slave
//         ret = i2c_read_blocking(I2C_PORT, I2C_SLAVE1_ADDR, rxdata, 16, false);
//         if (ret < 0) {
//             printf("Failed to read from slave\n");
//         } else {
//             printf("Received %d bytes from slave: ", ret);
//             for (int i = 0; i < ret; i++) {
//                 putchar(rxdata[i]);
//             }
//             putchar('\n');
//         }

//         // Read data from the slave
//         ret = i2c_read_blocking(I2C_PORT, I2C_SLAVE2_ADDR, rxdata, 16, false);
//         if (ret < 0) {
//             printf("Failed to read from slave\n");
//         } else {
//             printf("Received %d bytes from slave: ", ret);
//             for (int i = 0; i < ret; i++) {
//                 putchar(rxdata[i]);
//             }
//             putchar('\n');
//         }

//         // Wait before next communication
//         sleep_ms(1000);
//     }

//     return 0;
// }