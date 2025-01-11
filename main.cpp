#include "pindefs.h"
#include "i2c.h"
#include "led.h"
//#include "asm.h"
#include "shared_buf.h"

SharedBuffer mem_buf;

void core1_main() {
    while (1) {
        // printf("Core 1\n");
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(10000); // Wait for USB connection

    mutex_t shared_memory_mutex;
    //extern volatile uint8_t mem_buf[MEM_BUF_SIZE][MEM_BUF_ROW_SIZE]; 
    mutex_init(&shared_memory_mutex);
    multicore_launch_core1(core1_main);
    

    // Initialize I2C
    i2c_inst_t *i2c = I2CINSTANCE;  // Using I2C0 instance
    i2c_start_init(i2c);

    // Initialize LEDs
    led_init();

    uint8_t state = STATE_SCAN;
    uint8_t new_state;
    while (1) {
        
        //printf("State: %d\n", state);
        // printf("-----------------------------------\n");
        // new_state = asm_run(state);
        // state = new_state;

        //read neighbours and print them
        uint8_t neighbourss[4];
        //set values to 0
        for (int i = 0; i < 4; i++) {
            neighbourss[i] = 0;
        }
        read_neighbours(i2c, 9, neighbourss);
        printf("Neighbours: %d %d %d %d\n", neighbourss[0], neighbourss[1], neighbourss[2], neighbourss[3]);

        sleep_ms(500);
    }
    return 0;
}