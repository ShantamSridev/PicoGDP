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

    extern volatile uint8_t mem_buf[128][6]; 
    extern volatile uint8_t scan_buf[128];
    
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
        read_type(i2c, 0x09);
        // i2c_write(i2c, 0x09, 10, 50, WRITELENGTH);

        // uint8_t buf2[4];
        // i2c_read(i2c, 0x09, 10, buf2, READLENGTH); 

        // printf("Received: ");
        // for (int i = 0; i < 4; i++) {
        //     printf("%d ", buf2[i]);
        // }
        // printf("\n");
        sleep_ms(2000);
    }
    return 0;
}


//MAKE 128X6 MEM_BUF AND SCAN SHOULD FEED IN ADDRESSES TO THE MEM_BUF AND CHECK FOR CHANGE. SO A 128 SCAN_BUF WILL BE USED
// TO STORE THE PREVIOUS ADDRESSES AND COMPARE THEM TO THE CURRENT ADDRESSES. IF THERE IS A CHANGE THEN THE NEW ADDRESSES WILL 
//TYPE WILL BE CHECKED INTO MEM_BUF
//INTRODUCE STATE MACHINE FOR CONTINUOUS CHECK.