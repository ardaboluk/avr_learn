
/**
 * Ohm meter program using AVR.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <math.h>
#include "my_USART.h"

#define REF_VCC 4.9    // AVCC
#define REF_RESISTOR 983.0

// fake ISR for the empty ADC conversion complete interrupt
EMPTY_INTERRUPT(ADC_vect);

void initADC(void){
    // use AVCC as voltage reference and choose ADC5 as input channel
    ADMUX |= (1 << REFS0) | (1 << MUX2) | (1 << MUX0);
    // enable ADC, enable conversion complete intterupt and set prescaler to /16
    ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2);
    // set sleep mode to ADC noise reduction
    set_sleep_mode(SLEEP_MODE_ADC);
}

int main(void){

    float ohms = 0;
    uint16_t sampled_value = 0;
    float sampled_voltage = 0;
    
    // set up ADC5 for input
    DDRC &= ~(1 << PC5);

    // initialize USART0
    initUSART0();

    // initialize ADC and ADC sleep mode
    initADC();

    // enable interrupts
    sei();

    printStringUSART0("Digital Ohmmeter \r\n");

    while(1){

        sleep_mode();
        sampled_value = ADC;

        //DEBUG
        printFloat(sampled_value);
        printStringUSART0(" \r\n");

        sampled_voltage = sampled_value * (REF_VCC / 1024);

        //DEBUG
        printFloat(sampled_voltage);
        printStringUSART0(" volts\r\n");

        ohms = sampled_voltage * REF_RESISTOR / (REF_VCC - sampled_voltage);
        printFloat(ohms);
        printStringUSART0(" Ohms\r\n");
        _delay_ms(500);
    }

    return 0;
}