#include <stdio.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "i2c.h"
// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx

void core1_main() {
    while (1) {
        printf("Core 1\n");
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    multicore_launch_core1(core1_main);
    sleep_ms(10000); // Wait for USB connection if needed
    printf("I2C Bus Scan Initialised\n");

#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
#warning i2c/bus_scan example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    // Initialize I2C
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    while (1) {
        scan();
        sleep_ms(2000);
    }
    return 0;
#endif
}