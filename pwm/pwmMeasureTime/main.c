
#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include "my_timer.h"
#include "my_USART.h"

#ifndef BAUD
#define BAUD 9600
#endif

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED1 PB1
#define LED2 PB2
#define LED3 PB3

// if we give 20 for LED_DELAY_us and 255 for BRIGHTNESS_UPPER_LIMIT, timer shows 493 ms have been passed between dimmest and brightest
// if we give 5 for LED_DELAY_us and 510 for BRIGHTNESS_UPPER_LIMIT, timer shows 1083 ms have been passed between dimmest and brightest
// Firstly, these times are inconsistent with the values that we calculate (20*255*255 us = 1300 ms and 5*510*510 us = 1300 ms).
// Also, the second setting takes approximately twice as long as the first setting.
// Should solve these mysteries :o
#define LED_DELAY_us  5
#define BRIGHTNESS_UPPER_LIMIT 510

void pwmAllPins(uint16_t p_brightness); // PWM function. Sends one cycle of PWM wave to LEDs

int main(void) {

    uint16_t brightness = 0;
    int16_t direction = 1;
    uint16_t counterValue = 0;

    initTimer1();
    initUSART0();
    LED_DDR = 0xff;

    while (1) {
        if (brightness == 0){
            counterValue = TCNT1;
            // convert counter value to ms and transmit (remember each tick is 256 us)
            printTimeinMilliseondsUSART0(counterValue >> 2);
            TCNT1 = 0;
            direction = 1;
        }
        if (brightness == BRIGHTNESS_UPPER_LIMIT){
            counterValue = TCNT1;
            // convert counter value to ms and transmit (remember each tick is 256 us)
            printTimeinMilliseondsUSART0(counterValue >> 2);
            TCNT1 = 0;
            direction = -1;
        }
        brightness += direction;
        pwmAllPins(brightness);
    }

    return 0;
}

void pwmAllPins(uint16_t brightness){
    uint16_t i;
    LED_PORT |= (1 << LED1) | (1 << LED2) | (1 << LED3);
    for (i = 0; i < BRIGHTNESS_UPPER_LIMIT; i++){
        if (i >= brightness){
            LED_PORT &= ~((1 << LED1) | (1 << LED2) | (1 << LED3));
        }
        _delay_us(LED_DELAY_us);
    }
}
