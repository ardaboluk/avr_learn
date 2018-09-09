
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// pin defines
#define LEDS_DDR DDRB
#define LEDS_PORT PORTB
#define LED1 PB1
#define LED2 PB2
#define LED3 PB3
#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN PIND
#define BUTTON PD2

// if leave the parameter list blank in FUNCTION PROTOTYPE, this means that
// the function can take any number of parameters
void initUSART0(void);  // intialize USART0
void initTimer1(void);  // initialize Timer1
void printStringUSART0(const char p_string[]);  // transmit string over USART0
void printTime(uint16_t p_time);    // convert given time to string and transmit over USART0
void printComments(uint16_t p_time);    // transmit some comment string over USART0 regarding the given time
void randomDelay(void); // wait an unpredictable amount of time

int main(void){

    LEDS_DDR |= (1 << LED1) | (1 << LED2) | (LED3); // LED bits are defined as output
    LEDS_PORT &= ~((1 << LED1) | (1 << LED2) | (LED3)); // LEDS are turned off initially
    BUTTON_DDR &= ~(1 << BUTTON);   // BUTTON bit is defined as input
    BUTTON_PORT |= (1 << BUTTON);   // enable pull-up resistor

    // initialize USART0
    initUSART0();

    // initialize Timer1
    initTimer1();

    // the main event loop
    while(1){

    }

    // the code never reaches here
    return 0;
}
