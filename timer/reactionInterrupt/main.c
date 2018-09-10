
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

// global variables
uint8_t timer1Started = 0;   // made 1 when timer1 is launched and made 0 when ISR is complete

// if leave the parameter list blank in FUNCTION PROTOTYPE, this means that
// the function can take any number of parameters
void initUSART0(void);  // intialize USART0
void initTimer1(void);  // initialize Timer1
void enablePCI2(void);  // enable pin change interrupt 2
void disablePCI2(void); // disable pin change interrupt 2
uint8_t receiveByteUSART0(void);    // loop until data is recevied over USART0
void printStringUSART0(const char p_string[]);  // transmit string over USART0
void printTime(uint16_t p_time);    // convert given time to string and transmit over USART0
void printComments(uint16_t p_time);    // transmit some comment string over USART0 regarding the given time
void randomDelay(void); // wait an unpredictable amount of time

ISR(PCINT2_vect){
    // here, we don't have to debounce the button
    // we don't even check if the button is pressed or not
    // knowing that a change occurred is enough

    // we don't have to disable global interrupt flag, it's already disabled since
    // we're in an ISR
    uint16_t timePressed = TCNT1;
    printTime(timePressed);
    printComments(timePressed);
    timer1Started = 0;
    disablePCI2();
    // clear pin change interrupt 2 flag before exiting in case ISR doesn't
    // complete before the user releases the button. It's important to know
    // that an interrupt flag is cleared WHEN THE CORRESPONDING INTERRUPT IS
    // EANBLED. Thus, the flag can be set while the ISR is executing.
    PCIFR |= (1 << PCIF2);
}

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
        if(timer1Started == 0){
            LEDS_PORT &= ~((1 << LED1) | (1 << LED2) | (1 << LED3));
            printStringUSART0("Press a key to continue..\n");
            receiveByteUSART0();
            randomDelay();
            // check if the button is already pressed for cheating
            if((BUTTON_PIN & (1 << BUTTON)) == 0){
                printStringUSART0("No cheating :)\n");
                continue;
            }
            printStringUSART0("Go!\n");
            LEDS_PORT |= (1 << LED1) | (1 << LED2) | (1 << LED3);
            // reset timer1 (this should be an atomic operation, refer to datasheet)
            cli();
            TCNT1 = 0;
            sei();
            timer1Started = 1;
            // enable PCI2
            enablePCI2();
            }
        }
    }

    // the code never reaches here
    return 0;
}
