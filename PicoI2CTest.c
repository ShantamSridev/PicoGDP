#include "pindefs.h"
#include "i2c.h"
#include "led.h"


void core1_main() {
    while (1) {
        // printf("Core 1\n");
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    multicore_launch_core1(core1_main);
    sleep_ms(10000); // Wait for USB connection if needed
    
    // Initialize I2C
    i2c_inst_t *i2c = i2c0;  // Using I2C0 instance
    i2c_start_init(i2c);

    // Initialize LEDs
    led_init();
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    gpio_put(6, 1);
    gpio_init(7);
    gpio_set_dir(7, GPIO_OUT);
    gpio_put(7, 1);

    while (1) {
        i2c_scan(i2c);

        i2c_write(i2c, 0x09, 10, 50, WRITELENGTH);

        uint8_t buf2[4];
        i2c_read(i2c, 0x09, 10, buf2, READLENGTH); 

        printf("Received: ");
        for (int i = 0; i < 4; i++) {
            printf("%d ", buf2[i]);
        }
        printf("\n");
        sleep_ms(2000);
    }
    return 0;
}