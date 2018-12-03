
#include <avr/io.h>
#include "mytimer.h"

/**
* For Fast PWM there are 2 values. The TOP value and the compare value.
* The TOP value effects the timer and the pin output while the compare value effects only the pin output.
* The TOP value can be fixed values like 8-bit (255), 9-bit (511) or 10-bit (1023) for Timer1. Or it can 
* be determined by OCR1A or ICR1. The compare value is determined by OCR1x. If OCR1A is used for determining
* the TOP value, it can't be used for the compare value. Then, we can use OCR1B for the copare value.

* In non-inverting mode, TCNT1 value starts from BOTTOM and counts up to TOP. Along the way, if a compare match
* occurs (TCNT1 = OCR1x), OCN1x is cleared. TCNT1 starts from BOTTOM again and OC1x is set.
**/

void initTimer1(void){

    // set prescaler to clk_IO / 8
    TCCR1B |= (1 << CS11);
    
    // fast PWM mode. 8-bit resolution.
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);
    
    // clear OC1A on compare match and set at BOTTOM (non-inverting mode)
    TCCR1A |= (1 << COM1A1);        
}
