// blink led every second
// PB0(D8 of Arduino)->Resistor-> LED->GND

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 3000000UL // clock speed, used by delay.h
#endif
#include <util/delay.h>

// 1 sec if F_CPU set up correctly.
#define MS_DELAY 1000 

int main (void) {

    DDRB |= 0x01;// PB0 as OUTPUT

    PORTB &= 0xFE; // PB0 to 0

    while(1) {
        PORTB ^= 0x01; // toggle PB0
        _delay_ms(MS_DELAY);
    }
}
