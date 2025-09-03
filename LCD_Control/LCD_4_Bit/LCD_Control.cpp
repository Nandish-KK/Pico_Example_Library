/* This example shows the bare minimum setup required to run a LCD Display based on HD4478 LCD Driver.
The following example is configured to run with a 4-bit interface. It still need access to RS and EN pins.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "LCD_Control.h"
// #include "LCD_Control.h"
#define RS 0
#define RW 1
/* #define DB0 2
#define DB1 3
#define DB2 4
#define DB3 5 */

#define DB4 6
#define DB5 7
#define DB6 8
#define DB7 9
#define EN 10

#define BIT_H(n) (1u << (n))
#define BIT_L(n) (0u << (n))

uint toggle_ms = 5; // Enable toggle delay in ms
int value = 0;

/* map the parsed nibble data to GPIO mask
GPIO is set true if the (val & 1) returns true */
static inline uint32_t map_to_pins(uint8_t val)
{
    uint32_t bit_mask = 0;
    if (val & (1 << 0))
        bit_mask |= (1u << DB4);
    if (val & (1 << 1))
        bit_mask |= (1u << DB5);
    if (val & (1 << 2))
        bit_mask |= (1u << DB6);
    if (val & (1 << 3))
        bit_mask |= (1u << DB7);

    return bit_mask;
}

void EN_toggle()
{
    gpio_put(EN, 1);
    sleep_ms(toggle_ms);
    gpio_put(EN, 0);
}

/* uint8_t Data_bit -> 8-bit Data
   bool reg         -> Instruction/Data flag*/
void bit_parse(uint8_t Data_bit, bool reg)
{
    uint8_t up_bits = (0xF0 & Data_bit) >> 4;
    uint8_t lo_bits = 0x0F & Data_bit;

    uint32_t mask = 0x00 | map_to_pins(up_bits);
    gpio_put_all(mask);
    if (reg == true)
        gpio_put(RS, true);
    EN_toggle();

    mask = 0x00 | map_to_pins(lo_bits);
    gpio_put_all(mask);
    if (reg == true)
        gpio_put(RS, true);
    EN_toggle();
}

int main()
{
    stdio_init_all();
    gpio_init_mask(BIT_H(RS) | BIT_H(RW) | BIT_H(EN) | BIT_H(DB4) | BIT_H(DB5) | BIT_H(DB6) | BIT_H(DB7)); 
    gpio_set_dir_all_bits(BIT_H(RS) | BIT_H(RW) | BIT_H(EN) | BIT_H(DB4) | BIT_H(DB5) | BIT_H(DB6) | BIT_H(DB7));
    gpio_put(EN, 0);

    /* Function SET -> to sets 4 bit mode*/
    uint32_t mask = 0x00 | BIT_L(DB7) | BIT_L(DB6) | BIT_H(DB5) | BIT_L(DB4);
    gpio_put_all(mask); //
    gpio_put(EN, 1);
    sleep_ms(toggle_ms);
    gpio_put(EN, 0);

    /* Function SET */
    bit_parse(0x28, false);

    /* Display ON*/
    bit_parse(0xC, false);

    /* Clear Display*/
    bit_parse(CLR_DP, false);

    /* Entry Mode Set*/
    bit_parse(0x06, false);

    while (true)
    {

        char buffer[50];
        sprintf(buffer, "Time since boot: ");

        for (int i = 0; i < strlen(buffer); i++)
        {
            bit_parse(buffer[i], true);
        }

        while (true)
        {
            bit_parse(SET_RAM_ADDR(0x10), false); //Set DDRAM address to 0x10, this moves the cursor to 3rd line in a 16x4 Display.
            sprintf(buffer, "%d seconds", value);
            for (int i = 0; i < strlen(buffer); i++)
            {
                bit_parse(buffer[i], true);
            }
            value++;
            sleep_ms(1000);
        }

        bit_parse(RETURN_HOME, false); // Return home Instruction
    }
}
