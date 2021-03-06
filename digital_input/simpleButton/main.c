
#include <avr/io.h>
#include <util/delay.h>

int main(void){
    
    DDRD = 0x00;
    PORTD |= (1 << PD2);
    
    DDRB = 0xff;
    
    while(1){
        
        if(!(PIND & (1 << PD2))){
            PORTB = 1 << PB1;
        }else{
            PORTB = 1 << PB2;
        }
        
        /*
        // More readable version with bit_is_clear macro in avr/io.h
        if(bit_is_clear(PIND, PD2)){
            PORTB = 1 << PB1;
        }else{
            PORTB = 1 << PB2;
        }
        */
    }
    
    return 0;    
}
