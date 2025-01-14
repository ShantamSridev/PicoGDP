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

void init_pwm_two_pins_32ms_50pct(uint pinA, uint pinB) {
    // 1) Set GPIO pins to PWM function
    gpio_set_function(pinA, GPIO_FUNC_PWM);
    gpio_set_function(pinB, GPIO_FUNC_PWM);

    // 2) Both pins must be on the same slice for a perfectly in-sync output
    uint slice_num = pwm_gpio_to_slice_num(pinA);
    // Optionally verify slice_num == pwm_gpio_to_slice_num(pinB)

    // 3) Configure the PWM wrap (top)
    //    31.25 Hz = (125 MHz / clk_div) / (wrap + 1)
    pwm_set_wrap(slice_num, 19999);

    // 4) Set clock divider
    pwm_set_clkdiv(slice_num, 200.0f);

    // 5) Set 50% duty cycle on both channels
    uint channelA = pwm_gpio_to_channel(pinA);
    uint channelB = pwm_gpio_to_channel(pinB);
    pwm_set_chan_level(slice_num, channelA, 10000);
    pwm_set_chan_level(slice_num, channelB, 10000);

    // 6) Enable the PWM slice
    pwm_set_enabled(slice_num, true);
}

int main() {
    stdio_init_all();
    sleep_ms(5000); // Wait for USB connection

    mutex_t shared_memory_mutex;
    //extern volatile uint8_t mem_buf[MEM_BUF_SIZE][MEM_BUF_ROW_SIZE]; 
    mutex_init(&shared_memory_mutex);
    multicore_launch_core1(core1_main);
    

    // Initialize I2C
    i2c_inst_t *i2c = I2CINSTANCE;  // Using I2C0 instance
    i2c_start_init(i2c);

    // Initialize LEDs
    led_init(); 
    //enable led

    led_on(LED_1_B);
    led_on(LED_2_B);

    //state machine
    uint8_t state = STATE_SCAN;
    uint8_t new_state;

    init_pwm_two_pins_32ms_50pct(20, 21);


    while (1) {

        printf("-----------------------------------\n");
        printf("State: %d\n", state);
        new_state = asm_run(state);
        state = new_state;

        //scan i2c
        // uint8_t buf[128];   
        // i2c_scan(i2c, buf);

        //turn off led on i2c
        //write_live_state(I2CINSTANCE, 10, LED_OFF);
        //write_live_state(I2CINSTANCE, 16, LED_OFF);
        
        
        // for (int i = 10; i < 17; i++) {
        // //read neighbours and print them
        //     uint8_t neighbourss[4];
        //     //set values to 0
        //     for (int i = 0; i < 4; i++) {
        //         neighbourss[i] = 0;
        //     }
        //     printf("i: %d\n", i);
        //     read_neighbours(i2c, i, neighbourss);
        //     printf("N1: %d %d %d %d\n", neighbourss[0], neighbourss[1], neighbourss[2], neighbourss[3]);
        // }
        // uint8_t neighbours2[4];
        // //set values to 0
        // for (int i = 0; i < 4; i++) {
        //     neighbours2[i] = 0;
        // }
        // read_neighbours(i2c, 9, neighbours2);
        // printf("N2: %d %d %d %d\n", neighbours2[0], neighbours2[1], neighbours2[2], neighbours2[3]);
        printf("END--------------------------------\n");
        sleep_ms(5000);
    }
    return 0;
}