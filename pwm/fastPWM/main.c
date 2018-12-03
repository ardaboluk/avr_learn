
#include <avr/io.h>
#include <util/delay.h>
#include "mytimer.h"

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED1 PB1

#define LED_DELAY_us 2050
#define BRIGHTNESS_UPPER_LIMIT 255

int main(void){

    initTimer1();
    
    LED_DDR |= (1 << LED1);
    LED_PORT |= (1 << LED1);
    //LED_PORT &= ~(1 << LED1);
    
    uint8_t brightness = 1;
    int8_t direction = 1;
    
    while(1){
        OCR1A = brightness;
        if(brightness == BRIGHTNESS_UPPER_LIMIT){
            direction = -1;
        }else if(brightness == 0){
            direction = 1;
        }
        brightness += direction;
        _delay_us(LED_DELAY_us);
    }
    
    return 0;
}
