
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

// define baud rate if not defined
#ifndef BAUD
#define BAUD 9600
#endif

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

// global variables
// this variable is modified by an ISR, so we should define it volatile
volatile uint8_t timer1Started = 0;   // made 1 when timer1 is launched and made 0 when ISR is complete

// if leave the parameter list blank in FUNCTION PROTOTYPE, this means that
// the function can take any number of parameters
void initUSART0(void);  // intialize USART0
void initTimer1(void);  // initialize Timer1
void enableButtonInterrupt(void);  // enable interrupt for the button pin
void disableButtonInterrupt(void); // disable interrupt for the button pin
uint8_t receiveByteUSART0(void);    // loop until data is recevied over USART0
void transmitByteUSART0(uint8_t p_byte);    // transmit the given byte over USART0
void printStringUSART0(const char p_string[]);  // transmit string over USART0
void printTime(uint16_t p_time);    // convert given time to string and transmit over USART0
void printComments(uint16_t p_time);    // transmit some comment string over USART0 regarding the given time
void randomDelay(void); // wait an unpredictable amount of time

ISR(PCINT2_vect){
    // here, we don't have to debounce the button
    // we don't even check if the button is pressed or not
    // knowing that a change occurred is enough
    // because only the button is connected to PCI2

    // we don't have to disable global interrupt flag, it's already disabled since
    // we're in an ISR
    uint16_t timePressed = TCNT1 >> 4; // get milliseconds
    printTime(timePressed);
    printComments(timePressed);
    timer1Started = 0;
    disableButtonInterrupt();
    // clear pin change interrupt 2 flag before exiting in case ISR doesn't
    // complete before the user releases the button. It's important to know
    // that an interrupt flag is cleared WHEN THE CORRESPONDING INTERRUPT IS
    // ENABLED. Thus, the flag can be set while the ISR is executing and stay
    // set when the ISR returns.
    PCIFR |= (1 << PCIF2);
}

int main(void){
    LEDS_DDR |= (1 << LED1) | (1 << LED2) | (1 << LED3); // LED bits are defined as output
    LEDS_PORT &= ~((1 << LED1) | (1 << LED2) | (1 << LED3)); // LEDS are turned off initially
    BUTTON_DDR &= ~(1 << BUTTON);   // BUTTON bit is defined as input
    BUTTON_PORT |= (1 << BUTTON);   // enable pull-up resistor
    PCICR |= (1 << PCIE2);  // enable PCI2

    // initialize USART0
    initUSART0();

    // initialize Timer1
    initTimer1();

    // the main event loop
    while(1){
        if(timer1Started == 0){
            LEDS_PORT &= ~((1 << LED1) | (1 << LED2) | (1 << LED3));
            printStringUSART0("Press a key to continue..\r\n");
            receiveByteUSART0();
            randomDelay();
            // check if the button is already pressed for cheating
            if((BUTTON_PIN & (1 << BUTTON)) == 0){
                printStringUSART0("No cheating :)\r\n");
                continue;
            }
            printStringUSART0("Go!\r\n");
            LEDS_PORT |= (1 << LED1) | (1 << LED2) | (1 << LED3);
            // reset timer1 (this should be an atomic operation, refer to datasheet)
            cli();
            TCNT1 = 0;
            sei();
            timer1Started = 1;
            // enable PCI2 for the button
            enableButtonInterrupt();
        }
    }

    // the code never reaches here
    return 0;
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

void initTimer1(void){
    // set the prescaler to /64
    // CPU speed is defined as 1MHz in the makefile, thus the speed of the clock
    // is (1/64)MHz. Each tick is 64 microseconds long.
    TCCR1B = (1 << CS11) | (1 << CS10);
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

void printTime(uint16_t p_time){

    // itoa and sprintf are a bit controversial with small memory
    // so, i implemented int to str myself like the example of the book.
    // The right thing to do would be analyzing the assembly of itoa and sprintf
    // and use accordingly

    // 16-bit integer can be 65535 at most, so we start from 10000 to divide
    uint8_t digit = 0;
    while(p_time >= 10000){
        p_time -= 10000;
        digit++;
    }
    if(digit){
        transmitByteUSART0('0' + digit);
    }

    digit = 0;
    while(p_time >= 1000){
        p_time -= 1000;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    // p_time is in milliseconds but the time that will be sent is in seconds
    // So, a point is necessary after thousands
    transmitByteUSART0('.');

    digit = 0;
    while(p_time >= 100){
        p_time -= 100;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    digit = 0;
    while(p_time >= 10){
        p_time -= 10;
        digit++;
    }
    transmitByteUSART0('0' + digit);

    transmitByteUSART0('0' + (uint8_t)p_time);

    printStringUSART0(" seconds\r\n");

}

void printComments(uint16_t p_time){
    // i just copied the function from the example in the book
    if (p_time > 2000) {
        printStringUSART0("---->  Ummm...this is a reaction timer...\r\n");
    }
    else if (p_time > 1000) {
        printStringUSART0("---->  Hello?\r\n");
    }
    else if (p_time > 500) {
        printStringUSART0("---->  Slow.\r\n");
    }
    else if (p_time > 250) {
        printStringUSART0("---->  Have another cup of coffee.\r\n");
    }
    else if (p_time > 200) {
        printStringUSART0("---->  Respectable.\r\n");
    }
    else if (p_time >= 150) {
        printStringUSART0("---->  Fast.\r\n");
    }
    else if (p_time < 150) {
        printStringUSART0("---->  Amazing!\r\n");
    }
}

void randomDelay(void){
    // here reading the timer don't have to be atomic because we're not interested
    // in the time value, we're just seeking a random value.
    uint16_t randTime = TCNT1 >> 4;
    // we want to wait between one second and approximately 5 seconds
    // so we should get a value from the timer between 0 and 4000
    // if we right-shift the value of the timer 4 bits, we get a value that is
    // less than or equal to 4095
    // another note: _delay_ms expects a compile time int constant
    _delay_ms(1000);
    for(; randTime > 0; randTime--){
        _delay_ms(1);
    }
}
