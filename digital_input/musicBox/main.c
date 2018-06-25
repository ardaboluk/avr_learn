
#include <avr/io.h>
#include <util/delay.h>
#include "scale16.h"

#define SPEAKER_DDR DDRD
#define SPEAKER_PORT PORTD
#define SPEAKER_BIT PD6

#define BUTTON_DDR DDRC
#define BUTTON_PIN PINC
#define BUTTON_PORT PORTC
#define BUTTON_BIT PC3

#define DEBOUNCE_TIME_us 1000

#define DURATION 1600
#define SONG_LENGTH (sizeof(song) / sizeof(uint16_t))

uint8_t debounce();
void playNote(uint16_t period, uint16_t duration);
void rest(uint16_t duration);

int main(void){
    const uint16_t song[] = {
        E6, E6, E6, C6, E6, G6, G5,
        C6, G5, E5, A5, B5, Ax5, A5,
        G5, E6, G6, A6, F6, G6, E6, C6, D6, B5,
        C6, G5, E5, A5, B5, Ax5, A5,
        G5, E6, G6, A6, F6, G6, E6, C6, D6, B5
    };

    uint8_t whichNote = SONG_LENGTH - 1;
    uint8_t button_was_pressed = 0;

    SPEAKER_DDR |= (1 << SPEAKER_BIT);

    BUTTON_DDR &= ~(1 << BUTTON_BIT);
    BUTTON_PORT |= (1 << BUTTON_BIT);

    while(1){
        if(debounce()){
            if(button_was_pressed == 0){
                whichNote++;
                if(whichNote == SONG_LENGTH){
                    whichNote = 0;
                }
                button_was_pressed = 1;
            }
            playNote(song[whichNote], DURATION);
        }else{
            button_was_pressed = 0;
        }
    }

    return 0;
}

uint8_t debounce(){
    if((BUTTON_PIN & (1 << BUTTON_BIT)) == 0){
        _delay_us(DEBOUNCE_TIME_us);
        if((BUTTON_PIN & (1 << BUTTON_BIT)) == 0){
            return 1;
        }
    }
    return 0;
}

void playNote(uint16_t period, uint16_t duration){
    uint16_t elapsed;
    uint16_t i;
    for(elapsed = 0; elapsed < duration; elapsed += period){
        for(i = 0; i < period; i++){
            _delay_us(1);
        }
        SPEAKER_PORT ^= (1 << SPEAKER_BIT);
    }
}

void rest(uint16_t duration){
    do{
        _delay_us(1);
    }while(--duration);
}
