
#include <avr/io.h>
#include <util/delay.h>

// define the baud rate if not defined
#ifndef BAUD
#define BAUD 9600
#endif

// define pins, ports and ddrs
#define 

// initializes USART0
void initUSART0(){
    // initialization as in the datasheet
    unsigned int ubrr = BAUD;   // uint is 2 bytes in AVR
    // the most significant 4 bits are reserved and should be 0
    UBRR0H = 0x0f & (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)(ubrr);
    // enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // character size should be 8-bits
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// initializes ADC0
void initADC0(){
    // use AVCC as reference voltage
    ADMUX = (1 << REFS0);
    // set the prescaler clock division factor to /8
    ADCSRA = (1 << ADSP1) | (1 << ADSP0);
    // enable ADC
    ADCSRA |= (1 << ADEN);
}
