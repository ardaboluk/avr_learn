                                                       /* Blinker Demo */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */

#define TIME_DELAY 60


int main(void) {

  // -------- Inits --------- //
  DDRB |= 0b11111111;            /* Data Direction Register B:
                                   writing a one to the bit
                                   enables output. */

  // ------ Event loop ------ //
  while (1) {

    PORTB = 1;          
    _delay_ms(TIME_DELAY);     

    PORTB = 2;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 4;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 8;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 16;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 32;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 64;          
    _delay_ms(TIME_DELAY);
    
    PORTB = 128;          
    _delay_ms(TIME_DELAY);     

  }                     
  return 0;             
}
