
/*
*   This program checks whether an interrupt flag gets set or not when an
*   interrupt request happens if the corresponding interrupt is disabled.
*   Checking is done specifically for pin change interrupt. The answer is that
*   the interrupt flag will not get set when the interrupt is disabled.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

// define baud rate if not defined
#ifndef BAUD
#define BAUD 9600
#endif

#define TRUE 1
#define FALSE 0

// pin defines
#define LEDS_DDR DDRB
#define LEDS_PORT PORTB
#define LED0 PB1
#define LED1 PB2
#define LED2 PB3
#define BUTTON_DDR DDRD
#define BUTTON_PORT PORTD
#define BUTTON_PIN PIND
#define BUTTON PD2

#define button_pressed (BUTTON_PIN & (1 << BUTTON)) == 0
#define button_released !(button_pressed)

#define DEBOUNCE_DELAY_ms 5

volatile uint8_t is_button_pressed = FALSE;
volatile uint8_t is_button_released = FALSE;

uint8_t button_just_pressed_debounce(void);
uint8_t button_just_released_debounce(void);
void checkPCIFR(void);
void initUSART0(void);  // intialize USART0
void enableButtonInterrupt(void);  // enable interrupt for the button pin
void disableButtonInterrupt(void); // disable interrupt for the button pin
uint8_t receiveByteUSART0(void);    // loop until data is recevied over USART0
void transmitByteUSART0(uint8_t p_byte);    // transmit the given byte over USART0
void printStringUSART0(const char p_string[]);  // transmit string over USART0

ISR(PCINT2_vect){

    // check if the button is pressed or released
    if(button_just_pressed_debounce()){
        printStringUSART0("Button is just pressed.\r\n");
    }else if(button_just_released_debounce()){
        printStringUSART0("Button is just released.\r\n");
    }

    disableButtonInterrupt();
    printStringUSART0("Button interrupt mask is now cleared.\r\n");
    // clear pin change interrupt 2 flag before exiting in case ISR doesn't
    // complete before the user releases the button.
    //PCIFR |= (1 << PCIF2);
}

int main(void){
    LEDS_DDR = (1 << LED0) | (1 << LED1) | (1 << LED2); // LEDs are output
    BUTTON_DDR &= ~(1 << BUTTON);   // BUTTON bit is defined as input
    BUTTON_PORT |= (1 << BUTTON);   // enable pull-up resistor
    PCICR |= (1 << PCIE2);  // enable PCI2
    sei();  // enable global interrupts
    initUSART0();   // initialize USART0

    LEDS_PORT = (1 << LED0);
    printStringUSART0("Button interrupt mask is not set yet.\r\n");
    printStringUSART0("Press a key to start.\r\n");

    receiveByteUSART0();

    LEDS_PORT = (1 << LED1);
    checkPCIFR();
    enableButtonInterrupt();
    printStringUSART0("Button interrupt mask is set.\r\n");

    receiveByteUSART0();

    LEDS_PORT = (1 << LED2);
    checkPCIFR();

    printStringUSART0("While the interrupt mask is not set\r\n");
    printStringUSART0("the interrupt flag doesn't get set\r\n");
    printStringUSART0("even if an interrupt request happens.\r\n");
    printStringUSART0("Program finished.\r\n");
    cli();
    while(1);

    // the code never reaches here
    return 0;
}

uint8_t button_just_pressed_debounce(void){
    if(is_button_pressed == FALSE && button_pressed){
        _delay_ms(DEBOUNCE_DELAY_ms);
        if(button_pressed){
            is_button_pressed = TRUE;
            return TRUE;
        }
    }
    return FALSE;
}

uint8_t button_just_released_debounce(void){
    if(is_button_pressed == TRUE && button_released){
        _delay_ms(DEBOUNCE_DELAY_ms);
        if(button_released){
            is_button_pressed = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

void checkPCIFR(void){
    if((PCIFR & (1 << PCIF2)) == 0){
        printStringUSART0("PCIF2 is not set.\r\n");
    }else{
        printStringUSART0("PCIF2 is set.\r\n");
    }
}

void initUSART0(void){

    /* USART initilization is slightly more complicated than that's shown
    in the datasheet. For some baud rates required error rate may not be achievable.
    Thus, it may be required to set the speed to x2 by setting the U2X0 bit in
    UCSR0A. setbaud.h library provides this functionality. Its usage should be
    exactly like the one below.*/

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    // enable receiver and transmitter and set the character size to 8-bits
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void enableButtonInterrupt(void){
    PCMSK2 |= (1 << BUTTON);
}

void disableButtonInterrupt(void){
    PCMSK2 &= ~(1 << BUTTON);
}

uint8_t receiveByteUSART0(void){
    while((UCSR0A & (1 << RXC0)) == 0);
    return UDR0;
}

void transmitByteUSART0(uint8_t p_byte){
    while((UCSR0A & (1 << UDRE0)) == 0);
    UDR0 = p_byte;
}

void printStringUSART0(const char p_string[]){

    uint8_t i = 0;
    while(p_string[i]){
        transmitByteUSART0(p_string[i]);
        i++;
    }
}
