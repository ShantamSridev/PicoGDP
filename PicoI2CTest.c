#include "pindefs.h"
#include "i2c.h"
#include "led.h"
#include "asm.h"
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
        
        new_state = asm_run(state);
        state = new_state;

        sleep_ms(100);
    }
    return 0;
}


//MAKE 128X6 MEM_BUF AND SCAN SHOULD FEED IN ADDRESSES TO THE MEM_BUF AND CHECK FOR CHANGE. SO A 128 SCAN_BUF WILL BE USED
// TO STORE THE PREVIOUS ADDRESSES AND COMPARE THEM TO THE CURRENT ADDRESSES. IF THERE IS A CHANGE THEN THE NEW ADDRESSES WILL 
//TYPE WILL BE CHECKED INTO MEM_BUF
//INTRODUCE STATE MACHINE FOR CONTINUOUS CHECK.

// mutex_enter_blocking(&shared_memory_mutex);
//     // Write to mem_buf
//     mem_buf[0][0] = 42;
//     // Read from scan_buf
//     uint8_t value = scan_buf[0];
//     mutex_exit(&shared_memory_mutex);