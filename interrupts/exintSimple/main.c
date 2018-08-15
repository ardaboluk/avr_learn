
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LEDS_DDR DDRB
#define LEDS_PORT PORTB
#define LEDS_PIN PINB
#define LED0_BIT PB1
#define LED1_BIT PB2
#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN PIND
#define BUTTON_BIT PD2      // INT0 is on PD2 for atmega168

#define DEBOUNCE_DELAY 5

#define button_pressed (BUTTON_PIN & (1 << BUTTON_BIT)) == 0

uint8_t button_push_debounce(){
    if(button_pressed){
        _delay_ms(DEBOUNCE_DELAY);
        if(button_pressed){
            return 1;
        }
    }
    return 0;
}

// Runs every time there is a change on button
ISR(INT0_vect){
    if (button_push_debounce()) {
        LEDS_PORT |= (1 << LED1_BIT);
    }else{
        LEDS_PORT &= ~(1 << LED1_BIT);
    }
}

void initInterrupt0(void) {
    EIMSK |= (1 << INT0);   // enable INT0
    EICRA |= (1 << ISC00);  // trigger when button changes
    sei();  // set (global) interrupt enable bit
}

int main(void) {
    LEDS_DDR |= (1 << LED0_BIT) | (1 << LED1_BIT);
    BUTTON_DDR &= ~(1 << BUTTON_BIT);
    BUTTON_PORT |= (1 << BUTTON_BIT);   // activate the pull-up resistor on button pin
    initInterrupt0();

    while (1) {
        _delay_ms(200);
        LEDS_PORT ^= (1 << LED0_BIT);
    }
    return 0;
}
