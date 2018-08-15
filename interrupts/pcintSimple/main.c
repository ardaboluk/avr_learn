
// Pin change interrupt (PCINT) example with two buttons and three LEDs.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TRUE 1
#define FALSE 0

#define LEDS_DDR DDRB
#define LEDS_PORT PORTB
#define LEDS_PIN PINB
#define BUTTONS_DDR DDRD
#define BUTTONS_PORT PORTD
#define BUTTONS_PIN PIND
#define LED0_BIT PB1
#define LED1_BIT PB2
#define LED2_BIT PB3
#define BUTTON0_BIT PD6
#define BUTTON1_BIT PD7

#define button0_pressed (BUTTONS_PIN & (1 << BUTTON0_BIT)) == 0
#define button0_released (BUTTONS_PIN & (1 << BUTTON0_BIT)) == 1
#define button1_pressed (BUTTONS_PIN & (1 << BUTTON1_BIT)) == 0
#define button1_released (BUTTONS_PIN & (1 << BUTTON1_BIT)) == 1

#define DEBOUNCE_DELAY 5

// global variables to check state changes of the buttons
uint8_t is_button0_pressed = FALSE;
uint8_t is_button1_pressed = FALSE;

uint8_t button0_just_pressed_debounce(){
    if(is_button0_pressed == FALSE && button0_pressed){
        _delay_ms(DEBOUNCE_DELAY);
        if(button0_pressed){
            is_button0_pressed = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

uint8_t button1_just_pressed_debounce(){
    if(is_button1_pressed == FALSE && button1_pressed){
        _delay_ms(DEBOUNCE_DELAY);
        if(button1_pressed){
            is_button1_pressed = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

uint8_t button0_just_released_debounce(){
    if(is_button0_pressed == TRUE && button0_released){
        _delay_ms(DEBOUNCE_DELAY);
        if(button0_released){
            is_button0_pressed = FALSE;
            return 1;
        }
    }
    return 0;
}

uint8_t button1_just_released_debounce(){
    if(is_button1_pressed == TRUE && button1_released){
        _delay_ms(DEBOUNCE_DELAY);
        if(button1_released){
            is_button1_pressed = FALSE;
            return 1;
        }
    }
    return 0;
}

ISR(PCINT2_vect){
    if(button0_just_pressed_debounce()){
        LEDS_PORT |= (1 << LED1_BIT);
    }else if(button0_just_released_debounce()){
        LEDS_PORT &= ~(1 << LED1_BIT);
    }else if(button1_just_pressed_debounce()){
        LEDS_PORT |= (1 << LED2_BIT);
    }else if(button1_just_released_debounce()){
        LEDS_PORT &= ~(1 << LED2_BIT);
    }
}

void initPCI2(void){
    PCICR |= (1 << PCIE2);
    // we can either use the alias PCINTxx, or corresponding the pin number PDx etc.
    // the second one is more favorable here
    // PCMSK2 |= (1 << PCINT22) | (1 << PCINT23);
    PCMSK2 |= (1 << BUTTON0_BIT) | (1 << BUTTON1_BIT);
    sei();
}

int main(void){

    // LED pins are output
    LEDS_DDR |= (1 << LED0_BIT) | (1 << LED1_BIT) | (1 << LED2_BIT);
    // all LEDS are off at the beginning
    LEDS_PORT &= ~((1 << LED0_BIT) | (1 << LED1_BIT) | (1 << LED2_BIT));
    // button pins are input
    BUTTONS_DDR &= ~((1 << BUTTON0_BIT) | (1 << BUTTON1_BIT));
    // activate pull-up resistors for the button pins
    BUTTONS_PORT |= (1 << BUTTON0_BIT) | (1 << BUTTON1_BIT);

    //initialize PCI2
    initPCI2();

    // main event loop
    while(TRUE){
        LEDS_PORT ^= (1 << LED0_BIT);
        _delay_ms(500);
    }

    return 0;
}
