
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"

int getsw(){
    return (PORTD & 0xF00) >> 8;
}

int btns_p = 0;
int btns = 0;
int getbtns(){
    btns_p = btns;
    btns = ((PORTD & 0xE0) >> 4) | ((PORTF & 0x2) >> 1);
    return btns;
}
int is_clicked(int btnm){
    return (!(btns_p & btnm) && (btns & btnm));
}
int is_pressed(int btnm){
    return (btns & btnm);
}

int getpot(){
    /* Start sampling, wait until conversion is done */
    AD1CON1 |= (0x1 << 1);
    while(!(AD1CON1 & (0x1 << 1)));
    while(!(AD1CON1 & 0x1));
    return ADC1BUF0;
}
