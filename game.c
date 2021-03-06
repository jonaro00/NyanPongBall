
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"


// Time-keeping
uint32_t ticks = -1;     // Ticks since power on
int cooldown = 0;        // Used for state transitions
uint32_t tick_start = 1; // used for random seed
uint32_t level_started;  // time where current level started

// Special modes and debugging
uint8_t DEBUG_MODE = 0;
int *DEBUG_ADDR = &ticks;
int debug_dummy;
// Poll switch 4 for cheat mode
uint8_t cheat_mode(){
    return getsw() & BTN4; // Switch 4 up = cheat mode is on
}

// Game states
#define START 0
#define MENU 1
#define GAME 2
#define GAMEOVER 3
uint8_t game_state = START, prev_game_state = START;

// Level tracking
uint8_t levels_completed, level_progress, level_type, level_transition;
int8_t current_level;
uint16_t score_req[] = {0,15,30,50,70,100,150,200,250,300,400,500,600,700,800,900,1000,2000,3000};

// Menu values
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
char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.!_-#$";
char name[] = "AAA";
int name_pos;
int scrollwheel_offset; // The current position of the potentiometer
void set_scrollwheel_offset(){
    scrollwheel_offset = getpot()/32 - indexOf(name[name_pos], alphabet);
}
char *scoreboard;
int scoreboard_scroll, scoreboard_scroll_max;

// In-game score counter
int score;
float scoreY, scoreX;
char score_str[15] = "Score: ";

// Player
AnimUnit nyan;
#define NYAN_FLY_SPEED 0.6 // Fly speed in pong
#define NYAN_ACC_Y 0.125   // Gravity in dodgeball

// Pong units
AnimUnit ball;
#define BALL_MAX_SPEED_Y 1.2
float ball_prev_dy = -1, ball_prev_dx = -1;

// Dodgeball units
#define MAX_BALLS 6
AnimUnit balls[MAX_BALLS];
uint8_t n_balls;
Unit bullet;

// ### INTERRUPTS ### //
void interrupt_handler(){
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

// Gets run as often as possible
void loop(){
    // this turned out to be pretty useless
}

// Game logic every frame
void main_tick(){
    ticks++;

    // Get buttons pressed since last tick
    getbtns();

    // Start screen
    if(game_state == START){
        if(is_clicked(BTN4|BTN3|BTN2|BTN1)){ // wait for any button press
            game_state = MENU;
            tick_start = ticks;
        }
        screen_fill(1); // screen white
        screen_draw_box(2,2,SCREEN_HEIGHT-4,SCREEN_WIDTH-4,0); // black box in middle
        screen_display_string(6,23,"Nyan Pong Ball"); // title
        screen_display_string(21,60,"by jonaro00"); // subtitle
        screen_render();
        return;
    }

    // Debug pause check
    if(DEBUG_MODE && !is_clicked(BTN1)) return;

    // Has game state changed?
    uint8_t game_state_changed = prev_game_state != game_state;
    prev_game_state = game_state;
    // Has menu page changed?
    uint8_t menu_page_changed = prev_menu_page != menu_page || game_state_changed;
    prev_menu_page = menu_page;

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
                    if(is_clicked(BTN4)){name_pos = floorMod(name_pos-1, 3); set_scrollwheel_offset();}
                    if(is_clicked(BTN1)){name_pos = floorMod(name_pos+1, 3); set_scrollwheel_offset();}
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

// New game initialization
void game_init(){
    // Initial values for every game
    score = 0;
    current_level = -1;
    levels_completed = 0;
    level_transition = 0;
    // generate random backgrounds
    init_bg();
    // start at Pong level
    level_type0_init();
}

// Game logic every tick. Manages points & levels.
void game_tick(){
    if(getsw() & BTN3 && ticks % 5 == 0) score++;  // Switch 3 up = free points

    //# LEVEL PROGRESSION #//
    level_progress = 8*(score-score_req[current_level])/(score_req[current_level+1]-score_req[current_level]);
    PORTE = (0xff << 8-level_progress) & 0xff; // LEDS show progress towards next level
    if(level_transition && ticks / 10 % 2 == 0) // LEDS flash while waiting for next level to start
        PORTEINV = 0xff;
    if(score >= score_req[current_level+1] && !level_transition){ // score for next level is achieved.
        cooldown = 60; // 1s wait time
        level_transition = 1; // initiate move to next level
        levels_completed = (uint8_t)min(current_level+1, sizeof score_req);
    }

    //# MOVEMENT, COLLISIONS, GAMESTATE #//
    if(!level_type) level_type0_update();
    else            level_type1_update();

    //# GRAPHICS #//
    // Score counter is drawn regardless of level_type
    insert(itoaconv(score), score_str, 7, 1);
    screen_display_string(scoreY, scoreX, score_str);
    // Level-specific objects
    if(!level_type) level_type0_draw();
    else            level_type1_draw();
    // Player is drawn regardless of level_type
    draw_AnimUnit(&nyan);
}

// ### PONG LEVEL ### //

// Pong initialization
void level_type0_init(){
    // Set level-specific values
    level_type = 0;
    current_level++;
    cooldown = 20;
    scoreY = 12; scoreX = 50;
    // Set up units for current level
    init_AnimUnit(&ball,16,25,0.8F,1.7F+current_level*0.1F,0,0,11,11,&t_ball[0][0][0],1,BALL_FRAMES,32);
    init_AnimUnit(&nyan,14,0,0,0,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);
}
// Pong update cycle
void level_type0_update(){
    // Check for transition to next level
    if(ball.x > SCREEN_WIDTH+80 && level_transition){
        if(cooldown-- == 0){
            level_transition = 0;
            level_type1_init();
            return;
        }
    }
    // Background
    update_star_bg();
    // Player controls
    if(is_pressed(BTN3)) // press W (fly up)
        nyan.y = max(nyan.y-NYAN_FLY_SPEED, 0);
    if(is_pressed(BTN2)) // press S (fly down)
        nyan.y = min(nyan.y+NYAN_FLY_SPEED, SCREEN_HEIGHT-nyan.h);
    if(is_clicked(BTN4)) // click A (turn left)
        nyan.xdir = -1;
    if(is_clicked(BTN1)) // click D (turn right)
        nyan.xdir = 1;

    // Initial wait before the ball is served
    if(!level_transition){
        if(cooldown > 0){
            cooldown--;
            return;
        }
    }
    // Ball movement
    if     (ball.y < 1)                       ball.dy = abs(ball.dy); // top wall bounce
    else if(ball.y+ball.h >= SCREEN_HEIGHT-1) ball.dy = -abs(ball.dy); // bot wall bounce
    float yd = (ball.y+(ball.h-1)/2)-(nyan.y+(nyan.h-1)/2); // difference between ball center and nyan center
    if(ball.x+ball.w > SCREEN_WIDTH-1 && !level_transition) ball.dx = -abs(ball.dx); // right wall bounce
    else if(abs(ball.x - (nyan.x+nyan.w-3)) < 2 && // ball x at nyan nose
            abs(yd) < nyan.h/2 &&                  // ball center within nyan y
            ball.dx < 0)                           // ball going left
    {   // Ball & nyan collision
        ball.dx = -ball.dx; // dx is flipped
        float mdy = 0; // modification to dy
        if(!((ball.dy < 0 && yd <= 1) || (ball.dy > 0 && yd >= -1))) // ball hit on closer side
            mdy = yd / 3;
        else if((ball.dy < 0 && yd <= -2) || (ball.dy > 0 && yd >= 2)) // ball hit on further side
            mdy = yd / 5;
        ball.dy = bound(-BALL_MAX_SPEED_Y, ball.dy+mdy, BALL_MAX_SPEED_Y); // Add mdy and constrain to max speed

        score++;
    }
    // change spin direction and speed
    if(sign(ball_prev_dy) != sign(ball.dy) || sign(ball_prev_dx) != sign(ball.dx)){
        ball.xdir = ixor(ball.dy, ball.dx);
        ball.period = 48-40*abs(ball.dy/BALL_MAX_SPEED_Y);
    }
    ball_prev_dy = ball.dy; ball_prev_dx = ball.dx;

    move_Unit(&ball);

    // Death condition
    if(ball.x < -40){
        if(cheat_mode()) ball.x += 170;
        else{
            game_state = GAMEOVER;
            return;
        }
    }
}
// Pong draw routine
void level_type0_draw(){
    // Background
    draw_star_bg();
    // Walls
    screen_draw_box(0,30,1,SCREEN_WIDTH-30,1);
    screen_draw_box(SCREEN_HEIGHT-1,30,1,SCREEN_WIDTH-30,1);
    if(!level_transition)
        screen_draw_box(0,SCREEN_WIDTH-1,SCREEN_HEIGHT,1,1);
    // Ball
    draw_AnimUnit(&ball);
}

// ### DODGEBALL LEVEL ### //

// Dodgeball initialization
void level_type1_init(){
    // Level-specific values
    level_type = 1;
    current_level++;
    level_started = ticks;
    n_balls = min(3+current_level/2, MAX_BALLS);
    cooldown = 20;
    scoreY = 0; scoreX = 0;
    // Deactivate leftovers from previous levels
    uint8_t i;
    for(i = 0; i < MAX_BALLS; i++)
        balls[i].active = 0;
    bullet.active = 0;
    // Set player
    init_AnimUnit(&nyan,SCREEN_HEIGHT-15,(SCREEN_WIDTH-23)/2,0,0,0.125F,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);
}
// Dodgeball update cycle
void level_type1_update(){
    // Death check
    if(!nyan.alive && !unit_on_screen(&nyan)){
        game_state = GAMEOVER;
        return;
    }

    // Background
    update_rain_bg();

    // Transition check
    uint8_t i;
    for(i = 0; i < n_balls; i++)
        if(balls[i].active)
            break;
    // if no balls are active and transition is ongoing, run countdown towards next level
    if(i == n_balls && level_transition){
        if(cooldown-- == 0){
            level_transition = 0;
            level_type0_init();
            return;
        }
    }

    // Player controls
    if(is_clicked(BTN3)) // click W (jump)
        if(nyan.y == SCREEN_HEIGHT-nyan.h-1)
            nyan.dy = -2.2F;
    if(is_clicked(BTN2)) // click S (fast fall)
        nyan.dy = 1.3F;
    if(is_clicked(BTN4)) // click A (turn left / shoot)
        if(nyan.xdir == 1)
            nyan.xdir = -1;
        else if(!bullet.active)
            init_Unit(&bullet, nyan.y+5, nyan.x-5, 0, -0.5F, 0, -0.1F, 7, 8, &t_bullet[0][0], -1);
    if(is_clicked(BTN1)) // click D (turn right / shoot)
        if(nyan.xdir == -1)
            nyan.xdir = 1;
        else if(!bullet.active)
            init_Unit(&bullet, nyan.y+5, nyan.x+nyan.w-3, 0, 0.5F, 0, 0.1F, 7, 8, &t_bullet[0][0], 1);

    // MOVE NYAN & BULLET
    move_Unit(&nyan);
    nyan.y = min(nyan.y, SCREEN_HEIGHT-nyan.h-1);
    move_Unit(&bullet);

    // MOVEMENT, DEATH & COLLISIONS FOR ALL BALLS
    for(i = 0; i < n_balls; i++){
        AnimUnit * b = &balls[i];
        if(!b->active) continue;

        move_Unit(b);

        // Is ball OOB and dead/past the stage?
        if(!unit_on_screen(b)){
            if(!b->alive || ((b->x < 0 && b->dx < 0) || (b->x > 0 && b->dx > 0))){
                b->active = 0;
            }
        }

        // COLLISIONS
        // dead balls are skipped for collisions
        if(!b->alive) continue;
        // Nyan touches ball
        if(collides(&nyan, b)){
            if(nyan.y+nyan.h-1 - b->y < 3 && nyan.alive){ // Nyan jumped on ball
                nyan.dy = min(nyan.dy, -1);
                b->ay = 0.25F;
                b->alive = 0;
                score++;
            }
            else if(!cheat_mode()){ // Death
                nyan.alive = 0;
                nyan.dy = -2; nyan.ay = 0;
                nyan.dx = sign(b->dx)*3;
                return;
            }
        }
        // Bullet hits ball
        else if(collides(&bullet, b) && bullet.active){
            b->dy = -1.5F;
            b->dx = bullet.dx * 1.3F;
            b->alive = 0;
            bullet.active = 0;
            score++;
        }
    }
    // Bullet OOB
    if(bullet.active && !unit_on_screen(&bullet))
        bullet.active = 0;
    // OBJECT SPAWNING
    if(!level_transition){ // Don't spawn when next level score is achieved
        if(cooldown > 0){ // Start of level peace period
            cooldown--;
            return;
        }
        if((ticks-level_started) % 14 == 13){ // frequency of spawn chances
            if(random() < 0.2F){ // chance of spawn
                // spawn position
                int bxdir = random() < 0.55F ? -1 : 1;
                float h = random() < 0.75F ? SCREEN_HEIGHT-12 : 3;
                // spawn one if slot availible
                for(i = 0; i < n_balls; i++){
                    if(!balls[i].active){
                        init_AnimUnit(&balls[i],h,bxdir==1?-11:SCREEN_WIDTH,0,bxdir*(0.4F+current_level*0.1F),0,0,11,11,&t_ball[0][0][0],bxdir,BALL_FRAMES,10);
                        break;
                    }
                }
            }
        }
    }
}
// Dodgeball draw routine
void level_type1_draw(){
    // Background
    draw_rain_bg();
    // Ground
    screen_draw_box(SCREEN_HEIGHT-1,0,1,SCREEN_WIDTH,1);
    // Balls
    uint8_t i;
    for(i = 0; i < n_balls; i++)
        draw_AnimUnit(&balls[i]);
    // Bullet
    draw_Unit(&bullet);
}

// MENU PAGES VALUES & UPDATES
void gameover_init(){
    cooldown = 70; // Time to display gameover screen
    PORTECLR = 0xff; // Shut off LEDs
}
void gameover_tick(){
    screen_display_string(10, 30, "Game over!");
    if(cooldown--) return;
    game_state = MENU;
    menu_page = MENU_GAMEOVER;
}
void menu_main_init(){
    init_AnimUnit(&ball,20,89,0,0,0,0,11,11,&t_ball[0][0][0],1,BALL_FRAMES,8);
    init_AnimUnit(&nyan,17,64,0,0,0,0,14,23,&t_nyancat[0][0][0],1,NYANCAT_FRAMES,4);
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
    name_pos = 0; // selected character
    set_scrollwheel_offset();
}
void menu_gameover_tick(){
    screen_draw_box(0,56,SCREEN_HEIGHT,1,1);
    screen_display_string(0, 59, score_str);
    screen_display_string(8, 59, "Enter name:");
    name[name_pos] = alphabet[floorMod(getpot()/32 - scrollwheel_offset, 32)];
    screen_display_string(20, 82, name);
    screen_draw_box(28,82+name_pos*6,1,6,1); // selected character underline marker
}
