
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

/*  if we give 20 for LED_DELAY_us and 255 for BRIGHTNESS_UPPER_LIMIT, timer shows 1975 ms have been passed between dimmest and brightest
*   if we give 5 for LED_DELAY_us and 510 for BRIGHTNESS_UPPER_LIMIT, timer shows 4332 ms have been passed between dimmest and brightest
*   Firstly, these times are inconsistent with the values that we calculate (20*255*255 us = 1300 ms and 5*510*510 us = 1300 ms).
*   Also, the second setting takes approximately twice as long as the first setting.
*
*   It's more appropriate to resolve the second issue first. It's true that in both of the settings delay_us makes the system wait equal amounts of time.
*   However, in the second setting the if condition in pwmAllPins is checked 510 * 510 times in total. Also, the expression in the if block is called 510 times in total.
*   In the first setting the if condition is checked 255 * 255 times. The expression in the if block is called 255 times. So if we calculate these roughly m+n calls for
*   the first setting, it would be 4m+2n calls for the second setting. This gives a direction as to why the second setting takes slightly more than twice time as the first setting.
*
*   The first issue is caused also by this problem of overlooking the time if block takes. It should take 1300 ms. But the if block (together with the expression inside) takes some time.
*   In the second setting, it takes further time because of the larger number of calls.
*/

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
