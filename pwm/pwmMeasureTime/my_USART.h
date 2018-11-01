
#ifndef MY_USART_H
#define MY_USART_H

void initUSART0(void);  // initialize USART0
uint8_t receiveByteUSART0(void);    // receive byte via USART0
void transmitByteUSART0(uint8_t p_byte);    // transmit byte via USART0
void printStringUSART0(const char p_string[]);  // transmit string via USART0
void printTimeinMilliseondsUSART0(uint16_t p_time_ms);  // transmit given time via USART0
void printTimeinSecondsUSART0(uint16_t p_time_ms);  // convert given time to seconds and transmit via USART0


#endif
