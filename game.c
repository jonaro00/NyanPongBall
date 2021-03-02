
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
uint8_t levels_completed, level_progress, level_type, level_transition;
int8_t current_level;
uint16_t score_req[] = {0, 15, 30, 50, 70, 100, 150, 200};

#define MENU_MAIN 0
#define MENU_SCORES 1
#define MENU_GAMEOVER 2
uint8_t menu_page = MENU_MAIN, prev_menu_page = MENU_MAIN;

char menu_choices[][CHAR_SPACES] = {
    // MENU_MAIN
    "4-A Scores\n3-W \n2-S \n1-D Play",
    // MENU_SCORES
    "4-A Back\n3-W Up\n2-S Down\n1-D Play",
    // MENU_GAMEOVER
    "4-A Left\n3-W Skip\n2-S Enter\n1-D Right",
};
const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.!_-#$";
char name[] = "AAA";
int name_pos;
int scroll_offset;
void set_scroll_offset(){
    scroll_offset = getpot()/32 - indexOf(name[name_pos], alphabet);
}
char * scoreboard;
int scoreboard_scroll, scoreboard_scroll_max;


AnimUnit ball;
#define BALL_MAX_SPEED_Y 1.2
float ball_prev_dy = -1, ball_prev_dx = -1;
AnimUnit nyan;
#define NYAN_FLY_SPEED 0.6
int score;
float scoreY, scoreX;
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
                    if(is_clicked(BTN1)){game_state = GAME; return;}
                    if(menu_page_changed) menu_scores_init();
                    if(is_pressed(BTN3)) scoreboard_scroll = max(scoreboard_scroll-1, 0);
                    if(is_pressed(BTN2)) scoreboard_scroll = min(scoreboard_scroll+1, scoreboard_scroll_max);
                    menu_scores_tick();
                    break;
                case MENU_GAMEOVER:
                    if(is_clicked(BTN3)){menu_page = MENU_MAIN; return;}
                    if(is_clicked(BTN2)){add_Score(init_Score(name, score)); menu_page = MENU_MAIN; return;}
                    if(menu_page_changed) menu_gameover_init();
                    if(is_clicked(BTN4)){name_pos = floorMod(name_pos-1, 3); set_scroll_offset();}
                    if(is_clicked(BTN1)){name_pos = floorMod(name_pos+1, 3); set_scroll_offset();}
                    menu_gameover_tick();
                    break;
                default:
                    break;
            }
            // Print menu choices
            screen_display_string(0, 0, menu_choices[menu_page]);
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
    current_level = -1;
    levels_completed = 0;
    init_bg();
    level_type0_init();
}
void game_tick(){
    CHEAT_MODE = getsw() & BTN4; // Switch 4 up = cheat mode

    //# LEVEL PROGRESSION #//
    level_progress = 8*(score-score_req[current_level])/(score_req[current_level+1]-score_req[current_level]);
    if(level_progress) PORTESET = (1 << 8-level_progress);
    else               PORTECLR = 0xff;
    if(score >= score_req[current_level+1] && !level_transition){
        // initiate move to next level
        level_transition = 1;
        levels_completed = (uint8_t)min(current_level+1, sizeof score_req);
    }

    //# MOVEMENT, COLLISIONS, GAMESTATE #//
    // Background
    update_bg();
    // Level objects
    if(!level_type) level_type0_update();
    else            level_type1_update();

    //# GRAPHICS #//
    // Score counter
    insert(itoaconv(score), score_str, 7, 1);
    screen_display_string(scoreY, scoreX, score_str);
    // Background
    draw_bg();
    // Level objects
    if(!level_type) level_type0_draw();
    else            level_type1_draw();
    // Player
    draw_AnimUnit(&nyan);

    game_ticks++;
}
// Pong
void level_type0_init(){
    level_type = 0;
    current_level++;
    scoreY = 12; scoreX = 50;
    init_AnimUnit(&ball,16,40,0.8F,1.7F+current_level*0.2,11,11,&t_ball[0][0][0],1,BALL_FRAMES,32);
    init_AnimUnit(&nyan,16,0,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);
}
void level_type0_update(){
    if(ball.x > SCREEN_WIDTH+80 && level_transition){
        level_transition = 0;
        level_type1_init();
        return;
    }
    // Player
    if(is_clicked(BTN4)) btn_click(4);
    if(is_clicked(BTN1)) btn_click(1);
    if(is_pressed(BTN3)) btn_hold(3);
    if(is_pressed(BTN2)) btn_hold(2);
    // Ball
    if     (ball.y < 1)                       ball.dy = abs(ball.dy); // top wall bounce
    else if(ball.y+ball.h >= SCREEN_HEIGHT-1) ball.dy = -abs(ball.dy); // bot wall bounce
    float yd = (ball.y+(ball.h-1)/2)-(nyan.y+(nyan.h-1)/2);
    if(ball.x+ball.w > SCREEN_WIDTH-1 && levels_completed%2==0) ball.dx = -abs(ball.dx); // right wall bounce
    else if(abs(ball.x - (nyan.x+nyan.w-3)) < 2 && // ball x at nyan nose
            abs(yd) < nyan.h/2 &&                  // ball center within nyan y
            ball.dx < 0)                           // ball going left
    {   // Ball & nyan collision
        ball.dx = -ball.dx;
        float mdy = 0;
        if(!((ball.dy < 0 && yd <= 1) || (ball.dy > 0 && yd >= -1)))
            mdy = yd / 3;
        else if((ball.dy < 0 && yd <= -2) || (ball.dy > 0 && yd >= 2))
            mdy = yd / 5;
        ball.dy = bound(-BALL_MAX_SPEED_Y, ball.dy+mdy, BALL_MAX_SPEED_Y);

        score++;
    }
    // change spin direction and speed
    if(sign(ball_prev_dy) != sign(ball.dy) || sign(ball_prev_dx) != sign(ball.dx)){
        ball.xdir = ixor(ball.dy, ball.dx);
        ball.period = 48-40*abs(ball.dy/BALL_MAX_SPEED_Y);
    }
    ball_prev_dy = ball.dy; ball_prev_dx = ball.dx;

    move_Unit(&ball);
    if(CHEAT_MODE && ball.x < -30) ball.x += 170;

    if(ball.x < -40){game_state = GAMEOVER; return;}
}
void level_type0_draw(){
    // Walls
    screen_draw_box(0,30,1,SCREEN_WIDTH-30,1);
    screen_draw_box(SCREEN_HEIGHT-1,30,1,SCREEN_WIDTH-30,1);
    if(!level_transition)
        screen_draw_box(0,SCREEN_WIDTH-1,SCREEN_HEIGHT,1,1);

    draw_AnimUnit(&ball);
}
// Dodgeball
void level_type1_init(){
    level_type = 1;
    current_level++;
    scoreY = 0; scoreX = 0;
    init_AnimUnit(&nyan,SCREEN_HEIGHT-15,(SCREEN_WIDTH-23)/2,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);

}
void level_type1_update(){
    if(is_clicked(BTN4)) btn_click(4);
    if(is_clicked(BTN3)) btn_click(3);
    if(is_clicked(BTN2)) btn_click(2);
    if(is_clicked(BTN1)) btn_click(1);
}
void level_type1_draw(){
    // Ground
    screen_draw_box(SCREEN_HEIGHT-1,0,1,SCREEN_WIDTH,1);

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
            nyan.y = max(nyan.y-NYAN_FLY_SPEED, 0);
            break;
        case 2: // S
            nyan.y = min(nyan.y+NYAN_FLY_SPEED, SCREEN_HEIGHT-nyan.h);
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
    PORTECLR = 0xff;
    DEBUG_ADDR = &cooldown;
}
void gameover_tick(){
    screen_display_string(10, 30, "Game over!");
    if(cooldown--) return;
    game_state = MENU;
    menu_page = MENU_GAMEOVER;
}
void menu_main_init(){
    init_AnimUnit(&ball,20,89,0,0,11,11,&t_ball[0][0][0],1,BALL_FRAMES,8);
    init_AnimUnit(&nyan,17,64,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);
}
void menu_main_tick(){
    screen_draw_box(0,61,SCREEN_HEIGHT,1,1);
    screen_display_string(0,   67, "Nyan");
    screen_display_string(10,  87, "Pong");
    screen_display_string(20, 107, "Ball");
    draw_AnimUnit(&ball);
    draw_AnimUnit(&nyan);
}
void menu_scores_init(){
    scoreboard = get_scores_page();
    scoreboard_scroll = 0;
    scoreboard_scroll_max = max(0, (get_scores_len()-4)*8);
}
void menu_scores_tick(){
    screen_draw_box(0,50,SCREEN_HEIGHT,1,1);
    screen_display_string(-scoreboard_scroll, 52, scoreboard);
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
