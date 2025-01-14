#include "led.h"

void led_init(){
    gpio_init(LED_1_B);
    gpio_set_dir(LED_1_B, GPIO_OUT);

    gpio_init(LED_1_R);
    gpio_set_dir(LED_1_R, GPIO_OUT);

    gpio_init(LED_1_G);
    gpio_set_dir(LED_1_G, GPIO_OUT);

    gpio_init(LED_2_B);
    gpio_set_dir(LED_2_B, GPIO_OUT);

    gpio_init(LED_2_R);
    gpio_set_dir(LED_2_R, GPIO_OUT);

    gpio_init(LED_2_G);
    gpio_set_dir(LED_2_G, GPIO_OUT);

    // Set all LEDs to ON (active low, so write 0)
    gpio_put(LED_1_B, 1);
    gpio_put(LED_1_R, 1);
    gpio_put(LED_1_G, 1);
    gpio_put(LED_2_B, 1);
    gpio_put(LED_2_R, 1);
    gpio_put(LED_2_G, 1);

}

void led_on(uint8_t led){
    gpio_put(led, 0);
}

void led_off(uint8_t led){
    gpio_put(led, 1);
}

void all_leds_on(){
    gpio_put(LED_1_B, 0);
    gpio_put(LED_1_R, 0);
    gpio_put(LED_1_G, 0);
    gpio_put(LED_2_B, 0);
    gpio_put(LED_2_R, 0);
    gpio_put(LED_2_G, 0);
}

void all_leds_off(){
    gpio_put(LED_1_B, 1);
    gpio_put(LED_1_R, 1);
    gpio_put(LED_1_G, 1);
    gpio_put(LED_2_B, 1);
    gpio_put(LED_2_R, 1);
    gpio_put(LED_2_G, 1);
}

//make method to breathe pwm the two leds on white (all three colours same value)
void pwm_white(uint8_t led){
    for (int i = 0; i < 100; i++){
        gpio_put(led, 0);
        sleep_ms(1);
        gpio_put(led, 1);
        sleep_ms(1);
    }
}
