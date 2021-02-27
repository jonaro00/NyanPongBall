
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"


#define FPS 60
uint64_t game_ticks = 0;

#define START 0
#define MENU 1
#define GAME 2
uint8_t game_state = START;

uint8_t DEBUG_MODE = 0;
int* DEBUG_ADDR = &game_state;
int debug_dummy;


Unit ball;
AnimUnit nyan;
#define NYAN_SPEED_Y 0.6
int score = 0;

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
    screen_fill(1); // screen white
    screen_draw_box(8,8,SCREEN_HEIGHT-16,SCREEN_WIDTH-16,0); // black box in middle
    screen_display_string(12,20,"Nyan Pong Ball");
    screen_render();

    init_Unit(&ball,16,40,0.8F,1.8F,11,11,&t_ball[0][0],1);
    init_AnimUnit(&nyan,16,0,0,0,14,23,&t_nyancat[0][0][0],-1,NYANCAT_FRAMES);
}
// ### INTERRUPTS ### //
void user_isr(){
    // TIMER -- main clock
    if(IFS(0) & 0x100){ // check timer interrupt bit (8)
        IFS(0) &= ~0x100; // clear the interrupt flag bit

        game_tick(); // try to advance the game
    }
    // SW1 - Toggle debug mode
    else if(IFS(0) & (0x80)){ // check SW1 interrupt bit
        IFS(0) &= ~(0x80); // clear int bit
        DEBUG_MODE = DEBUG_MODE == 0 ? 1 : 0; // toggle debug mode
    }
}
void loop(){
    //
}

void game_tick(){

    // Get buttons pressed since last game_tick
    getbtns();

    // Start screen check
    if(game_state == START){
        if(is_clicked(BTN4|BTN3|BTN2|BTN1))
            game_state = GAME;
        return;
    }

    // Debug pause check
    if(DEBUG_MODE && !is_clicked(BTN1)) return;

    switch(game_state){
        case MENU:
            break;

        case GAME:
            // ### MOVEMENT & COLLISIONS ### //
            // Player
            if(is_clicked(BTN4)) btn_press(4);
            if(is_clicked(BTN3)) btn_press(3);
            if(is_clicked(BTN2)) btn_press(2);
            if(is_clicked(BTN1)) btn_press(1);
            if(is_pressed(BTN4)) btn_hold(4);
            if(is_pressed(BTN3)) btn_hold(3);
            if(is_pressed(BTN2)) btn_hold(2);
            if(is_pressed(BTN1)) btn_hold(1);
            // Ball
            if     (ball.y < 1)                       ball.dy = abs(ball.dy); // top wall bounce
            else if(ball.y+ball.h >= SCREEN_HEIGHT-1) ball.dy = -abs(ball.dy); // bot wall bounce
            float yd = (ball.y+(ball.h-1)/2)-(nyan.y+(nyan.h-1)/2);
            if(ball.x+ball.w > SCREEN_WIDTH-1 && ball.dx > 0) ball.dx = -ball.dx; // right wall bounce
            else if(abs(ball.x - (nyan.x+nyan.w-3)) < 2 && // ball x at nyan nose
                    abs(yd) < nyan.h/2 &&                  // ball center within nyan y
                    ball.dx < 0)                           // ball going left
            {   // Ball & nyan collision
                ball.dx = -ball.dx;
                if(!((ball.dy < 0 && yd <= 1) || (ball.dy > 0 && yd >= -1)))
                    ball.dy = bound(-1.2, ball.dy+yd/2, 1.2);
            }
            ball.x += ball.dx; ball.y += ball.dy;
            ball.x += ball.x < -30 ? 170 : 0; // noob mode

            // ### GRAPHICS ### //
            screen_draw_box(0,30,1,SCREEN_WIDTH,1); // top wall
            screen_draw_box(SCREEN_HEIGHT-1,30,1,SCREEN_WIDTH,1); // bot wall
            screen_draw_box(0,SCREEN_WIDTH-1,SCREEN_HEIGHT,1,1); // right wall


            draw_Unit(&ball);
            draw_AnimUnit(&nyan);
            game_ticks++;

            break;
        default:
            break; // lol wut
    }

    // Debug info goes on top of everything else
    if(DEBUG_MODE) print_debug(DEBUG_ADDR);

    // Render the frame
    screen_render();
}

void btn_press(int btn_i){
    switch(btn_i){
        case 3: // W
            break;
        case 2: // S
            break;
        case 4: // A
            break;
        case 1: // D
            break;
        default:
            break;
    }
}
void btn_hold(int btn_i){
    switch(btn_i){
        case 3: // W
            nyan.y -= NYAN_SPEED_Y;
            nyan.y = bound(0, nyan.y, SCREEN_HEIGHT-nyan.h);
            break;
        case 2: // S
            nyan.y += NYAN_SPEED_Y;
            nyan.y = bound(0, nyan.y, SCREEN_HEIGHT-nyan.h);
            break;
        case 4: // A
            nyan.xdir = -1;
            break;
        case 1: // D
            nyan.xdir = 1;
            break;
        default:
            break;
    }
}
