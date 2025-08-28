/**
 * Rotary Encoder implementation for Raspberry Pi Pico Microcontroller using C-SDK
 * Copyright 2025 Nandish Kadabagere Licenced under the GNU GPL Version 3.
 * 
 * This implementation uses state machine approach to detect the CW and CCW Rotation of a simple HW-040 Rotary Encoder.
 * 
 * The logic is pretty much derived from the buxtronix arduino Rotary encoder handler. The modified version from CarlosSiles67 is a bit 
   clear to follow. Follow the links below for more detail. 
 * CarlosSiles67 --> https://github.com/CarlosSiles67/Rotary
 * buxtronix     --> https://github.com/buxtronix/arduino/blob/master/libraries/Rotary
 * 
 * Check out blogpost from CarlosSiles. This goes in detail on the logic behind. --> https://carlossilesblog.wordpress.com/2019/07/27/rotary-encoder/
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico_rotary.h"

// GPIO pins
#define GPIO_RCLK_PIN 2
#define GPIO_RDT_PIN 3
#define GPIO_RSW_PIN 4
#define GPIO_LED_PIN 25

volatile uint32_t past_time = 0;
// volatile uint value = 0;

int main()
{
    stdio_init_all();

    printf("Hello Pico_Rotary\n");

    // Configure GPIO pins for BIT0 and BIT 1 of Rotary Encoder
    gpio_init(GPIO_RCLK_PIN);
    gpio_set_dir(GPIO_RCLK_PIN, GPIO_IN);

    gpio_init(GPIO_RDT_PIN);
    gpio_set_dir(GPIO_RDT_PIN, GPIO_IN);

    // Configure SW1
    gpio_init(GPIO_RSW_PIN);
    gpio_pull_up(GPIO_RSW_PIN);
    gpio_set_dir(GPIO_RSW_PIN, GPIO_IN);

    // Conigure GPIO for Toggle LED
    gpio_init(GPIO_LED_PIN);
    gpio_set_dir(GPIO_LED_PIN, GPIO_OUT);

    // Set IRQ for BIT0 and BIT1 Pins
    gpio_set_irq_enabled_with_callback(GPIO_RCLK_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(GPIO_RDT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Set IRQ for Rotary switch
    gpio_set_irq_enabled_with_callback(GPIO_RSW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Wait forever
    while (1)
        ;
}

uint Encoder_Event(uint gpio)
{
    // Debounce Setup.
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    uint32_t time_diff = current_time - past_time;
    bool SW_STAT = false;

    if (time_diff < debounce_delay_ms) // compare delay between states and exit if its less than debounce delay.
    {
        return 0;
    }

    if (gpio == GPIO_RSW_PIN)
    {
       printf("SWITCH PRESSED %d\n",current_time);
        past_time = current_time;
        return SW_STAT;
    }

    uint8_t pin_state = read_encoder_state();

    state = transition_table[state & 0xf][pin_state]; // Search for lookup table entry. Bitwise operation -> (state & 0xf) to mask DIR flag.
    uint8_t DIR = state & 0x30;                       // mask state bits to obtain DIR flag.

    if (DIR == DIR_CW) // Increment value if direction is clock-wise
    {
        // value++;
        // printf("CLOCKWISE CLICK: %d\n", value);
        past_time = current_time;
        return DIR;
    }

    else if (DIR == DIR_CCW) // Decrement value if direction is counter-clock-wise
    {
        // value--;
        // printf("COUNTER-CLOCKWISE CLICK: %d\n", value);
        past_time = current_time;
        return DIR;
    }
}

encoder_state_t read_encoder_state(void)
{
    // Read the actual GPIO pins
    bool clk_pin = gpio_get(GPIO_RCLK_PIN); // Returns true/false
    bool dt_pin = gpio_get(GPIO_RDT_PIN);   // Returns true/false

    // Perform bit-wise operation to combine both pins
    uint8_t combined_value = (dt_pin << 1) | clk_pin;
    return (encoder_state_t)combined_value;
}

void gpio_callback(uint gpio, uint32_t events)
{
    // Callback
    uint irq_pin = gpio;
    Encoder_Event(irq_pin);
    gpio_put(GPIO_LED_PIN, !gpio_get(GPIO_LED_PIN)); // Toggle the LED state
}
