
#include <avr/io.h>
#include "my_timer.h"

void initTimer1(void){
    // set the prescaler to /256
    // CPU speed is defined as 1MHz in the makefile, thus the speed of the clock
    // is (1/256)MHz. Each tick is 256 microseconds long.
    TCCR1B = (1 << CS12);
}
