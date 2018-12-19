
#include <avr/io.h>
#include <util/setbaud.h>
#include <math.h>
#include "my_USART.h"

void initUSART0(void){

    /* USART initilization is slightly more complicated than that's shown
    in the datasheet. For some baud rates required error rate may not be achievable.
    Thus, it may be required to set the speed to x2 by setting the U2X0 bit in
    UCSR0A. setbaud.h library provides this functionality. Its usage should be
    exactly like the one below.*/

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    // enable receiver and transmitter and set the character size to 8-bits
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void transmitByteUSART0(uint8_t p_byte){
    while((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = p_byte;
}

void printStringUSART0(const char p_string[]){

    uint8_t i = 0;
    while(p_string[i]){
        transmitByteUSART0(p_string[i]);
        i++;
    }
}

void printFloat(float p_number){
    // first transmit the sign and make the number positive
    if(p_number < 0){
        transmitByteUSART0('-');
        p_number = fabs(p_number);
    }

    // round the number to 2 decimal places
    p_number = round(p_number * 100) / 100;

    // find the number of digits in the whole part
    uint8_t numDigits = 0;
    float curMultiplier = 10.0;
    while(floor(p_number / curMultiplier) != 0){
        numDigits++;
        curMultiplier *= 10.0;
    }
    numDigits++;

    // transmit each digit of the whole number over USART0
    float digit = 0;
    while(p_number > 1){
        digit = p_number / pow(10, --numDigits);
        transmitByteUSART0('0' + digit);
        p_number -= floor(digit) * pow(10, numDigits);
    }

    // place the decimal point
    transmitByteUSART0('.');

    // print two decimal places
    transmitByteUSART0('0' + (p_number * 10));
    transmitByteUSART0('0' + (p_number * 100) - floor(p_number * 10) * 10);
}
