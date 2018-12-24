
#ifndef MY_USART_H
#define MY_USART_H

void initUSART0(void);  // initialize USART0
void transmitByteUSART0(uint8_t p_byte);    // transmit byte via USART0
void printStringUSART0(const char p_string[]);  // transmit string via USART0
void printFloat(float p_number);    // round the floating point value to 2 decimal places and transmit it over USART0

#endif
