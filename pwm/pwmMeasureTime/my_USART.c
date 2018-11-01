
#include <avr/io.h>
#include <util/setbaud.h>
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

uint8_t receiveByteUSART0(void){
    while((UCSR0A & (1 << RXC0)) == 0);
    return UDR0;
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

void printTimeinMilliseondsUSART0(uint16_t p_time_ms){

    // 16-bit integer can be 65535 at most, so we start from 10000 to divide
    uint8_t digit = 0;
    while(p_time_ms >= 10000){
        p_time_ms -= 10000;
        digit++;
    }
    if(digit){
        transmitByteUSART0('0' + digit);
    }

    digit = 0;
    while(p_time_ms >= 1000){
        p_time_ms -= 1000;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    digit = 0;
    while(p_time_ms >= 100){
        p_time_ms -= 100;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    digit = 0;
    while(p_time_ms >= 10){
        p_time_ms -= 10;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    transmitByteUSART0('0' + (uint8_t)p_time_ms);

    printStringUSART0(" milliseconds\r\n");

}


void printTimeinSecondsUSART0(uint16_t p_time_ms){

    // itoa and sprintf are a bit controversial with small memory
    // so, i implemented int to str myself like the example of the book.
    // The right thing to do would be analyzing the assembly of itoa and sprintf
    // and use accordingly

    // 16-bit integer can be 65535 at most, so we start from 10000 to divide
    uint8_t digit = 0;
    while(p_time_ms >= 10000){
        p_time_ms -= 10000;
        digit++;
    }
    if(digit){
        transmitByteUSART0('0' + digit);
    }

    digit = 0;
    while(p_time_ms >= 1000){
        p_time_ms -= 1000;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    // p_time is in milliseconds but the time that will be sent is in seconds
    // So, a point is necessary after thousands
    transmitByteUSART0('.');

    digit = 0;
    while(p_time_ms >= 100){
        p_time_ms -= 100;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    digit = 0;
    while(p_time_ms >= 10){
        p_time_ms -= 10;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    transmitByteUSART0('0' + (uint8_t)p_time_ms);

    printStringUSART0(" seconds\r\n");

}
