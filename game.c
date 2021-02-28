
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"


uint32_t game_ticks = 0;
int cooldown = 0;

uint8_t DEBUG_MODE = 0;
int* DEBUG_ADDR = &game_ticks;
int debug_dummy;
uint8_t CHEAT_MODE = 0;

#define START 0
#define MENU 1
#define GAME 2
#define GAMEOVER 3
uint8_t game_state = START, prev_game_state = START;

#define MENU_MAIN 0
#define MENU_SCORES 1
#define MENU_GAMEOVER 2
uint8_t menu_page = MENU_MAIN, prev_menu_page = MENU_MAIN;

char menu_choices[][4][CHAR_SPACES] = {
    { // MENU_MAIN
        "4-A Scores",
        "3-W ",
        "2-S ",
        "1-D Play",
    },
    { // MENU_SCORES
        "4-A Back",
        "3-W Up",
        "2-S Down",
        "1-D Play",
    },
    { // MENU_GAMEOVER
        "4-A Left",
        "3-W Skip",
        "2-S Enter",
        "1-D Right",
    },
};
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.!_-#$";
char name[] = "AAA";
int name_pos;
int scroll_offset;
void set_scroll_offset(){
    scroll_offset = getpot()/32 - indexOf(name[name_pos], alphabet);
}


Unit ball;
AnimUnit nyan;
#define NYAN_SPEED_Y 0.6
int score;
char score_str[15] = "Score: ";

// ### INTERRUPTS ### //
void user_isr(){
    // TIMER -- main clock
    if(IFS(0) & 0x100){ // check timer interrupt bit (8)
        IFS(0) &= ~0x100; // clear the interrupt flag bit

        main_tick();
    }
    // SW1 - Toggle debug mode
    else if(IFS(0) & 0x80){ // check interrupt bit
        IFS(0) &= ~0x80; // clear int bit
        DEBUG_MODE = DEBUG_MODE == 0 ? 1 : 0;
    }
    // SW4 - Toggle cheat mode
    // else if(IFS(0) & 0x80000){
    //     IFS(0) &= ~0x80000;
    //     CHEAT_MODE = CHEAT_MODE == 0 ? 1 : 0;
    // }
}
void loop(){
    //
}

void main_tick(){
    // Get buttons pressed since last tick
    getbtns();

    // Has game state changed?
    uint8_t game_state_changed = prev_game_state != game_state;
    prev_game_state = game_state;

    uint8_t menu_page_changed = prev_menu_page != menu_page || game_state_changed;
    prev_menu_page = menu_page;

    // Start screen check
    if(game_state == START){
        if(is_clicked(BTN4|BTN3|BTN2|BTN1))
            game_state = MENU;
        screen_fill(1); // screen white
        screen_draw_box(8,8,SCREEN_HEIGHT-16,SCREEN_WIDTH-16,0); // black box in middle
        screen_display_string(12,23,"Nyan Pong Ball"); // title
        screen_render();
        return;
    }

    // Debug pause check
    if(DEBUG_MODE && !is_clicked(BTN1)) return;

    switch(game_state){
        case MENU:
            switch(menu_page){
                case MENU_MAIN:
                    if(is_clicked(BTN4)){menu_page = MENU_SCORES; return;}
                    if(is_clicked(BTN1)){game_state = GAME; return;}
                    if(menu_page_changed) menu_main_init();
                    menu_main_tick();
                    break;
                case MENU_SCORES:
                    if(is_clicked(BTN4)){menu_page = MENU_MAIN; return;}
                    if(is_clicked(BTN3)) ;
                    if(is_clicked(BTN2)) ;
                    if(is_clicked(BTN1)){game_state = GAME; return;}
                    if(menu_page_changed) menu_scores_init();
                    menu_scores_tick();
                    break;
                case MENU_GAMEOVER:
                    if(is_clicked(BTN4)){name_pos = floorMod(name_pos-1, 3); set_scroll_offset();}
                    if(is_clicked(BTN3)){menu_page = MENU_MAIN; return;}
                    if(is_clicked(BTN2)) ;
                    if(is_clicked(BTN1)){name_pos = floorMod(name_pos+1, 3); set_scroll_offset();}
                    if(menu_page_changed) menu_gameover_init();
                    menu_gameover_tick();
                    break;
                default:
                    break;
            }
            // Print menu choices
            int i;
            for(i = 0; i < 4; i++)
                screen_display_string(i*8, 0, menu_choices[menu_page][i]);
            break;

        case GAME:
            if(game_state_changed) game_init();
            game_tick();
            break;
        case GAMEOVER:
            if(game_state_changed) gameover_init();
            gameover_tick();
            break;
        default:
            screen_display_string(0,0,"Achievement unlocked:");
            screen_display_string(8,0,"How did we get here?");
            break;
    }

    // Debug info goes on top of everything else
    if(DEBUG_MODE) print_debug(DEBUG_ADDR);

    // Render the frame
    screen_render();
}

void game_init(){
    game_ticks = 0;
    score = 0;
    init_Unit(&ball,16,40,0.8F,1.7F,11,11,&t_ball[0][0],1);
    init_AnimUnit(&nyan,16,0,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES);
}
void game_tick(){
    CHEAT_MODE = getsw() & BTN4;

    // ### MOVEMENT & COLLISIONS ### //
    // Player
    if(is_clicked(BTN4)) btn_click(4);
    if(is_clicked(BTN3)) btn_click(3);
    if(is_clicked(BTN2)) btn_click(2);
    if(is_clicked(BTN1)) btn_click(1);
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
            abs(yd) < nyan.h/2)                    // ball center within nyan y
    {   // Ball & nyan collision
        ball.dx = abs(ball.dx);
        if(!((ball.dy < 0 && yd <= 1) || (ball.dy > 0 && yd >= -1)))
            ball.dy = bound(-1.2, ball.dy+yd/2, 1.2);
    }
    ball.x += ball.dx; ball.y += ball.dy;
    if(CHEAT_MODE) ball.x += ball.x < -30 ? 170 : 0;

    if(ball.x < -40){game_state = GAMEOVER; return;}

    // ### GRAPHICS ### //
    screen_draw_box(0,30,1,SCREEN_WIDTH,1); // top wall
    screen_draw_box(SCREEN_HEIGHT-1,30,1,SCREEN_WIDTH,1); // bot wall
    screen_draw_box(0,SCREEN_WIDTH-1,SCREEN_HEIGHT,1,1); // right wall

    char * ccc = itoaconv(score);
    int i = 7;
    while(*ccc != 0){
        score_str[i] = *ccc;
        ccc++; i++;
    }
    screen_display_string(12, 50, score_str);


    draw_Unit(&ball);
    draw_AnimUnit(&nyan);
    game_ticks++;
}

void btn_click(int btn_i){
    switch(btn_i){
        case 3: // W
            break;
        case 2: // S
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
            break;
        case 1: // D
            break;
        default:
            break;
    }
}
void gameover_init(){
    cooldown = 70;
    DEBUG_ADDR = &cooldown;
}
void gameover_tick(){
    screen_display_string(10, 30, "Game over!");
    if(cooldown--) return;
    game_state = MENU;
    menu_page = MENU_GAMEOVER;
}
void menu_main_init(){
    init_AnimUnit(&nyan,17,64,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES);
}
void menu_main_tick(){
    screen_draw_box(0,61,SCREEN_HEIGHT,1,1);
    screen_display_string(0,  63,  "Nyan");
    screen_display_string(8,  85,  "Pong");
    screen_display_string(16, 107, "Ball");
    draw_AnimUnit(&nyan);
}
void menu_scores_init(){

}
void menu_scores_tick(){

}
void menu_gameover_init(){
    name_pos = 0;
    set_scroll_offset();
}
void menu_gameover_tick(){
    screen_draw_box(0,56,SCREEN_HEIGHT,1,1);
    screen_display_string(0, 59, score_str);
    screen_display_string(8, 59, "Enter name:");
    name[name_pos] = alphabet[floorMod(getpot()/32 - scroll_offset, 32)];
    screen_display_string(20, 82, name);
    screen_draw_box(28,82+name_pos*6,1,6,1);
}
