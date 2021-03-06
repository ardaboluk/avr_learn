
#include <avr/io.h>
#include <util/delay.h>

#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN PIND

#define LED_DDR DDRB
#define LED_PORT PORTB

#define BUTTON_BIT PD2
#define LED_BIT PB1

#define DEBOUNCE_TIME_uS 1000

uint8_t debounce(){
    if((BUTTON_PIN & (1 << BUTTON_BIT)) == 0){
        _delay_us(DEBOUNCE_TIME_uS);
        if((BUTTON_PIN & (1 << BUTTON_BIT)) == 0){
            return 1;
        }
    }
    return 0;
}

int main(void){

    uint8_t button_was_pressed = 0;         // last state of the button

    BUTTON_DDR &= ~(1 << BUTTON_BIT);       // configure button gpio
    BUTTON_PORT |= (1 << BUTTON_BIT);       // enable the pullup resistor for button

    LED_DDR |= (1 << LED_BIT);              // configure LED gpio

    // main event loop
    while(1){

        if(debounce()){                     // debounced button press
            if(button_was_pressed == 0){
                LED_PORT ^= (1 << LED_BIT);
                button_was_pressed = 1;     // this could also come after this if block
            }
        }else{
            button_was_pressed = 0;
        }
    }

    return 0;                               // this line is never reached
}
