
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

    while (1) {
        i2c_scan(i2c);
        sleep_ms(2000);
    }
    return 0;
}