
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"


#define FPS 60

uint64_t game_ticks = 0;
uint8_t game_started = 0;

uint8_t DEBUG_MODE = 0;
int* DEBUG_ADDR = 0;


Unit ball1;
AnimUnit nyan;

void game_init(){
    // Configure timer 2
    T2CON = 0x70; // Clear timer 2 registers and set prescaler to 256 (0b111)
    PR2 = 80000000 / 256 / FPS; // Set period register to desired period
    TMR2 = 0; // Reset timer 2
    T2CONSET = 0x8000; // Start timer 2
    IEC(0) = (1 << 8); // Enable interrupts from timer 2
    IPC(2) = 4; // set priority for timer 2

    // EXT INT
    INTCONSET = 0b11111; // rising edge for ext int
    IEC(0) |= (1 << 7); // Enable interrupts from switch 1
    IPC(1) |= (1 << 26); // set priority for SW1
    enable_interrupt();

    // LEDS
    TRISECLR = 0xFF; // set lower 8 bits of port E to output (LEDS)
    PORTE = 0;
    ODCE = 0x0;

    // BTN & SW
    TRISDSET = 0xFE0; // set bits 11-5 of port D to input (btns and switches)
    TRISFSET = 0x2; // set bit 1 of port F to input (btn1)

    // ADC
    AD1PCFG = 0xFFFB; // PCFG2 = input
    TRISBSET = 4;


    // START SCREEN
    screen_draw_box(0,0,SCREEN_HEIGHT,SCREEN_WIDTH,1); // screen white
    screen_draw_box(8,8,SCREEN_HEIGHT-16,SCREEN_WIDTH-16,0); // black box in middle
    screen_display_string(12,20,"Nyan Pong Ball");
    screen_render();

    init_Unit(&ball1,16,40,0,0,11,11,&ball[0][0]);
    init_AnimUnit(&nyan,2,0,0,0,14,23,&nyancat[0][0][0], NYANCAT_FRAMES);
}
void user_isr( void )
{
    if(IFS(0) & 0x100){ // check timer interrupt bit (8)
        IFS(0) &= ~0x100; // clear the interrupt flag bit

        //######################
        //GAMETICK
        if(!DEBUG_MODE)
            game_tick();
        else
            debug_screen();


    }
    else if(IFS(0) & (0x80)){ // check SW1 interrupt bit
        IFS(0) &= ~(0x80); // clear int bit
        PORTE++;
        DEBUG_MODE = DEBUG_MODE == 0 ? 1 : 0; // toggle debug mode
    }
}
void loop(){
    // if(DEBUG_ADDR){
    //     print_debug(DEBUG_ADDR);
    //     screen_reset();
    // }
}

void DEBUG(int* a){
    DEBUG_ADDR = a;
    // debug_screen();

    while(!is_clicked(BTN1)){
        getbtns();
    }
}

void game_tick(){
    getbtns();
    if(!game_started){
        if(is_clicked(BTN4|BTN3|BTN2|BTN1)){game_started = 1;}
        return;
    }


    if(is_clicked(BTN4)){btn_press(4);}
    if(is_clicked(BTN3)){btn_press(3);}
    if(is_clicked(BTN2)){btn_press(2);}
    if(is_clicked(BTN1)){btn_press(1);}
    if(is_pressed(BTN4)){btn_hold(4);}
    if(is_pressed(BTN3)){btn_hold(3);}
    if(is_pressed(BTN2)){btn_hold(2);}
    if(is_pressed(BTN1)){btn_hold(1);}


    draw_Unit(&ball1);
    draw_AnimUnit(&nyan);

    screen_render();

    PORTE++;
    game_ticks++;
}

void btn_press(int btn_i){

    char q[] = {(btn_i+'0'), 0};
    screen_display_string(16, 120, q);
}
void btn_hold(int btn_i){

    char q[] = {(btn_i+'0'), 0};
    screen_display_string(8, 120, q);

    switch (btn_i){
        case 4:
            nyan.y += 1;
            break;
        case 2:
            nyan.y -= 1;
            break;
        default:
            break;
    }
}

int x = 0, y = 0, dx = 1, dy = 1;
void debug_screen(){

    // DVD screensaver
    x += dx; y += dy;
    if (x+24 >= 56 || x < 0) dx = -dx;
    if (y+8 >= SCREEN_HEIGHT || y < 0) dy = -dy;
    screen_display_string(y, x, "DVD");

    // DEBUG INFO
    print_debug(DEBUG_ADDR);

    screen_render();
}