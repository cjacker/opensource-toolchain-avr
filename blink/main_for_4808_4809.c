// Atmega 4808/4809
// Blink led connect to PF5 (A7 pin for Arduino nano every)

#include <avr/io.h>

#define F_CPU 3333333UL
#include <util/delay.h>

int main(void)
{
    PORTF.DIR |= PIN5_bm;

    while (1)
    {
        PORTF.OUT |= PIN5_bm;
        _delay_ms(200);
        PORTF.OUT &= ~PIN5_bm;
        _delay_ms(200);
    }

    return 0;
}
