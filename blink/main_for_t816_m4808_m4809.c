// Blink demo for Attiny 816 and Atmega 4808/4809(nano every)
// Led connect to PB0 (D9 pin for Arduino nano every)

#include <avr/io.h>
#define F_CPU 3330000UL
#include <util/delay.h>

int main(void)
{
    PORTB.DIR |= PIN0_bm;

    while (1)
    {
        PORTB.OUT |= PIN0_bm;
        _delay_ms(200);
        PORTB.OUT &= ~PIN0_bm;
        _delay_ms(200);
    }

    return 0;
}
