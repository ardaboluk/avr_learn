
/**
 * Ohm meter program using AVR.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <math.h>
#include "my_USART.h"

#define REF_VCC 4.83    // AVCC
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

uint16_t overSample16x(void){
    uint16_t oversampled_value = 0;
    uint8_t i = 0;
    for(; i < 16; i++){
        sleep_mode();
        oversampled_value += ADC;
    }
    return (oversampled_value >> 2);
}

int main(void){

    float ohms = 0;
    uint16_t oversampled_value = 0;
    float oversampled_voltage = 0;
    
    // set up ADC5 for input
    DDRC &= ~(1 << PC5);

    // initialize ADC and ADC sleep mode
    initADC();

    // enable interrupts
    sei();

    printStringUSART0("Digital Ohmmeter \r\n");

    while(1){

        oversampled_value = overSample16x();
        oversampled_voltage = oversampled_value * (REF_VCC / 4096);
        ohms = oversampled_voltage * REF_RESISTOR / (REF_VCC - oversampled_voltage);
        printFloat(ohms);
        printStringUSART0(" Ohms\r\n");
        _delay_ms(500);
    }

    return 0;
}