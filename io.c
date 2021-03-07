
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"

// Returns 4 bits with current states of the switches.
int getsw(){
    return (PORTD & 0xF00) >> 8;
}

int btns_p = 0;
int btns = 0;
// Returns 4 bits with current states of the buttons.
// Also updates which buttons were pressed/are being held.
int getbtns(){
    btns_p = btns;
    btns = ((PORTD & 0xE0) >> 4) | ((PORTF & 0x2) >> 1);
    return btns;
}
// Returns 1 if button was pressed since last fetch.
int is_clicked(int btnm){
    return (!(btns_p & btnm) && (btns & btnm));
}
// Returns 1 if button is currently held down.
int is_pressed(int btnm){
    return (btns & btnm);
}

// Returns 0-1024 based on the potentiometers position.
int getpot(){
    // Start sampling, wait until conversion is done
    AD1CON1 |= (0x1 << 1);
    while(!(AD1CON1 & (0x1 << 1)));
    while(!(AD1CON1 & 0x1));
    return ADC1BUF0;
}
