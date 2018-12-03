
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mytimer.h"

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED1 PB1

#define LED_DELAY_us 2050
#define BRIGHTNESS_UPPER_LIMIT 255

volatile uint8_t brightness = 0;
volatile int8_t direction = 1;

ISR(TIMER1_OVF_vect){
    OCR1A = brightness;
    if(brightness == BRIGHTNESS_UPPER_LIMIT){
        direction = -1;
    }else if(brightness == 0){
        direction = 1;
    }
    brightness += direction;
}

int main(void){

    initTimer1();
    
    LED_DDR |= (1 << LED1);
    LED_PORT &= ~(1 << LED1);

    // enable global interrupts
    sei();
    
    while(1){
        
    }
    
    return 0;
}