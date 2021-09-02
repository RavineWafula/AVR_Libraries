#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "ADC.h"

void ADC_initialization(){
    // make ADC port as input
    DDRC = 0x00;
    // enable ADC with a division factor of 128
    ADCSRA = 0b10000111;
    // AVcc with external capacitor at AREF pin
    ADMUX = 0b01000000;
}

int ADC_Read(char channel){
    int A_in, A_in_low;

    // set input channel to read
    ADMUX |= (channel & 0x0F);
    // start conversion
    ADCSRA |= (1<<ADSC);
    // monitor end of conversion interrupt
    while((ADCSRA & (1<<ADIF))==0);

    _delay_us(10);
    
    A_in_low = (int) ADCL;  // read low byte
    A_in = (int) ADCH*256;  // read higher 2 bits and multiply with weight

    A_in += A_in_low;

    return A_in;

}
