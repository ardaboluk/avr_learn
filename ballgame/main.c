
#include <avr/io.h>
#include <util/delay.h>

// define the baud rate if not defined
#ifndef BAUD
#define BAUD 9600
#endif

// define pins, ports and ddrs
#define POT_DDR DDRC
#define POT_PIN PINC
#define POT_PORT PORTC
#define BUTTON_DDR DDRD
#define BUTTON_PIN PIND
#define BUTTON_PORT PORTD
#define LEDS_DDR DDRB
#define LEDS_PIN PINB
#define LEDS_PORT PORTB
// define bits
#define POT_ANGLE_BIT PC0
#define POT_VELOCITY_BIT PC1
#define BUTTON_BIT PD6
#define LED_GO_BIT PB1
#define LED_STOP_BIT PB2
// debounce delay time (in ms)
#define DEBOUNCE_DELAY 5

// helper macro to check if the button is pressed
#define button_pressed (BUTTON_PIN & (1 << BUTTON_BIT)) == 0

//initializes pins and ports
void initPinsPorts(){
    // potentiometers are used as ADC input and thus the pull-up resistors shouldn't be activated
    POT_DDR &= ~((1 << POT_ANGLE_BIT) | (1 << POT_VELOCITY_BIT));
    POT_PORT &= ~((1 << POT_ANGLE_BIT) | (1 << POT_VELOCITY_BIT));
    // button is a digital input and thus the pull-up resistor should be activated
    BUTTON_DDR &= ~(1 << BUTTON_BIT);
    BUTTON_PORT |= (1 << BUTTON_BIT);
    // leds are output and are not lighted up here in the initialization
    LEDS_DDR |= (1 << LED_GO_BIT) | (1 << LED_STOP_BIT);
    LEDS_PORT &= ~((1 << LED_GO_BIT) | (LED_STOP_BIT));
}

// initializes USART0
void initUSART0(){
    // initialization as in the datasheet
    unsigned int ubrr = BAUD;   // uint is 16 bits in AVR
    // the most significant 4 bits are reserved and should be 0
    UBRR0H = 0x0f & (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)(ubrr);
    // enable receiver and transmitter and set the character size to 8-bits
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// initializes ADC0
void initADC0_leftAdjusted(){
    // use AVCC as reference voltage and left adjust the conversion result
    // as we'll need only leftmost 8 bits
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    // set the prescaler clock division factor to /8
    ADCSRA = (1 << ADSP1) | (1 << ADSP0);
    // enable ADC
    ADCSRA |= (1 << ADEN);
}

// button push check via debouncing
uint8_t button_push_debounce(){
    if(button_pressed){
        _delay_ms(DEBOUNCE_DELAY);
        if(button_pressed){
            return 1;
        }
    }
    return 0;
}

// receives 8-bits over USART0
uint8_t receiveByte_USART0(){

    // wait until there's unread data
    while((UCSR0A & (1 << RXC0)) == 0);
    // return the received data
    return UDR0;
}

// transmits 8-bits over USART0
void transmitByte_USART0(uint8_t data){

    // wait until the transmitter buffer is empty
    while(UCSR0A & (1 << UDRE0) == 0);
    // write the low byte to UDR0
    UDR0 = data;
}

// makes the ADC choose the given channel and returns the leftmost 8 bits of the conversion result
uint8_t readADC_leftmost8(uint8_t channel){
    ADMUX = ((ADMUX & 0xf0) | channel);
    ADCSRA |= (1 << ADSC);
    while(ADCSRA & (1 << ADSC));
    return ADCH;
}

int main(){

    // initialize necessary hardware and the ports
    initPinsPorts();
    initUSART0();
    initADC0_leftAdjusted();

    // last read potentiometer values
    uint8_t pot_angle_value = 0;
    uint8_t pot_velocity_value = 0;

    // data that's received from USART0
    // if 0 ball isn't thrown yet, if 1 ball is thrown
    uint8_t data_received = 0

    // data that will be transmitted through USART0
    // if leftmost 2 bits 00->pot_angle 01->pot_velocity 10->button pressed 11->do nothing
    uint8_t data_tobe_transmitted = 0;

    // main loop
    while(1){
        // receive 8-bits from USART0
        data_received = receiveByte_USART0();
        // if the ball isn't thrown yet
        if(data_received == 0){
            // turn the green LED on and red LED off
            LEDS_PORT = (1 << LED_GO_BIT);
            // read both of the potentiometer values from the ADC
            // we're using the low 7 bits of the ADC conversion result
            // for both of the potentiometers
            uint8_t current_pot_angle_value = readADC(POT_ANGLE_BIT);
            uint8_t current_pot_velocity_value = readADC(POT_VELOCITY_BIT);
            // also, check if the button is pressed
            uint8_t button_is_pressed = button_push_debounce();
            // set data to be transmitted
            if(button_is_pressed == 1){
                data_tobe_transmitted = 0x80;
            }else{
                if(current_pot_angle_value != pot_angle_value){
                    data_tobe_transmitted = current_pot_angle_value & 0x3f;
                    pot_angle_value = current_pot_angle_value;
                }else if(current_pot_velocity_value != pot_velocity_value){
                    data_tobe_transmitted = 0x40 | ((current_pot_velocity_value << 2) >> 2);
                    pot_velocity_value = current_pot_velocity_value;
                }else{
                    data_tobe_transmitted = 0xC0;
                }
            }
        }else{  // if the ball is thrown
            // turn the red LED on and green LED off
            LED_PORT = (1 << LED_STOP_BIT);
            data_tobe_transmitted = 0xC0;
        }
        // transmit the data
        transmitByte_USART0(data_tobe_transmitted);
    }
}
