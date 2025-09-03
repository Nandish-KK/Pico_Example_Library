#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
// #include "LCD_Control.h"
#define RS 0
#define RW 1
#define DB0 2
#define DB1 3
#define DB2 4
#define DB3 5
#define DB4 6
#define DB5 7
#define DB6 8
#define DB7 9
#define EN 10

#define BIT_H(n) (1u << (n))
#define BIT_L(n) (0u << (n))



int main()
{
    stdio_init_all();
    gpio_init_mask((1u << 11) - 1); // Left Shift 1 to 11th bit position and subtract it by 1. This generates 100000000000 --> 011111111111
    gpio_set_dir_all_bits((1u << 11) - 1);
    gpio_put(EN, 0);

    /*      Clear DISPLAY   */
    gpio_put_all(0x1 << 2); // only DB0 is High (offset by 2 since DB0 -> GPIO2)
    gpio_put(EN, 1);
    sleep_ms(100);
    gpio_put(EN, 0);

    /* Function SET*/
    gpio_put_all(0x7 << 5); // set DB4|DB3|DB2
    gpio_put(EN, 1);
    sleep_ms(100);
    gpio_put(EN, 0);

    /* DISPLAY ON*/
    gpio_put_all(0x7 << 3); // set DB3|DB2|DB1
    gpio_put(EN, 1);
    sleep_ms(100);
    gpio_put(EN, 0);

    /* Entry Mode*/
    gpio_put_all(0x3 << 3); // set DB2|DB1
    gpio_put(EN, 1);
    sleep_ms(100);
    gpio_put(EN, 0);

    /*     gpio_init_mask(0x00);
        gpio_set_mask(0x00) */
    ;

    while (true)
    {
        char Data[] = "HELLO WORLD";

        sleep_ms(500);
        for (int i = 0; i < strlen(Data); i++)
        {
            uint8_t bit_mask = Data[i];
            uint32_t pin_mask = bit_mask << 2 | 0x1;
            gpio_put_all(pin_mask);
            sleep_ms(50);
            gpio_put(EN, 1);
            sleep_ms(100);
            gpio_put(EN, 0);
        }
        return 0;
    }
}
