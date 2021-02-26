
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"

int getsw (void){
    return (PORTD & 0xF00) >> 8;
}

int btns_p = 0;
int btns = 0;
int getbtns (void){
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
