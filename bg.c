
#include <stdint.h>
#include "game.h"

#define ss 16
Unit small_stars[ss];
uint8_t t_ss[1][1] = {{1}};
#define st 10
Unit stars[st];
uint8_t t_s[2][2] = {{1,1},{1,1}};

void init_bg(){
    int i;
    for(i = 0; i < ss; i++){
        Unit s;
        init_Unit(&s, (int)abs((i%2==0?i:-i)*i*37/(ss/2-i+1.1F))%(SCREEN_HEIGHT-2)+1, floorMod(i*i*i*-185,ss*16),0,-0.3F,0,0,1,1,&t_ss[0][0],1);
        small_stars[i] = s;
    }
    for(i = 0; i < st; i++){
        Unit s;
        int ii = st - i;
        init_Unit(&s, (int)abs((ii%2==0?ii:-ii)*ii*37/(ss/2-ii+1.1F))%(SCREEN_HEIGHT-2)+1, floorMod(ii*ii*i*-185,ss*28),0,-0.5F,0,0,2,2,&t_s[0][0],1);
        stars[i] = s;
    }
}
void update_bg(){
    int i;
    Unit * s;
    for(i = 0; i < ss; i++){
        s = &small_stars[i];
        if(s->x < -10) s->x += ss*16;
        move_Unit(s);
    }
    for(i = 0; i < st; i++){
        s = &stars[i];
        if(s->x < -10) s->x += ss*28;
        move_Unit(s);
    }
}
void draw_bg(){
    int i;
    for(i = 0; i < ss; i++)
        draw_Unit(&small_stars[i]);
    for(i = 0; i < st; i++)
        draw_Unit(&stars[i]);
}
