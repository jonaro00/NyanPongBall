
#include <stdint.h>
#include "game.h"

// Arrays and textures for background particles
#define ss 16
Unit small_stars[ss];
uint8_t t_ss[1][1] = {{1}};
#define st 10
Unit stars[st];
uint8_t t_s[2][2] = {{1,1},{1,1}};
#define rd 40
Unit drops[rd];
uint8_t t_rd[3][1] = {{1},{1},{1}};

// Randomly places stars and raindrops on background (can be outside of screen).
void init_bg(){
    int i;
    Unit s;
    for(i = 0; i < ss; i++){
        init_Unit(&s, random()*(SCREEN_HEIGHT-2)+1, random()*ss*16,0,-0.3F+random()*-0.1F,0,0,1,1,&t_ss[0][0],1);
        small_stars[i] = s;
    }
    for(i = 0; i < st; i++){
        init_Unit(&s, random()*(SCREEN_HEIGHT-2)+1, random()*st*28,0,-0.5F+random()*-0.1F,0,0,2,2,&t_s[0][0],1);
        stars[i] = s;
    }
    for(i = 0; i < rd; i++){
        init_Unit(&s, random()*rd*4, random()*SCREEN_WIDTH-1,0.8F+random()*0.3F,0,0,0,3,1,&t_rd[0][0],1);
        drops[i] = s;
    }
}
// Moves all stars horizontally.
void update_star_bg(){
    int i;
    Unit *s;
    for(i = 0; i < ss; i++){
        s = &small_stars[i];
        if(s->x < -1) s->x += ss*16;
        move_Unit(s);
    }
    for(i = 0; i < st; i++){
        s = &stars[i];
        if(s->x < -1) s->x += st*28;
        move_Unit(s);
    }
}
// Draws all stars.
void draw_star_bg(){
    int i;
    for(i = 0; i < ss; i++)
        draw_Unit(&small_stars[i]);
    for(i = 0; i < st; i++)
        draw_Unit(&stars[i]);
}
// Moves all raindrops vertically.
void update_rain_bg(){
    int i;
    Unit *s;
    for(i = 0; i < rd; i++){
        s = &drops[i];
        if(s->y > SCREEN_HEIGHT) s->y -= rd*4;
        move_Unit(s);
    }
}
// Draws all raindrops.
void draw_rain_bg(){
    int i;
    for(i = 0; i < rd; i++)
        draw_Unit(&drops[i]);
}
