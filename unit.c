
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"

void init_Unit(Unit * u, int y, int x, int dy, int dx, uint8_t h, uint8_t w, uint8_t * texture){
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->h = h;
    u->w = w;
    u->texture = texture;
}
void draw_Unit(Unit * u){
    screen_display_texture(u->y, u->x, u->h, u->w, u->texture);
}

void init_AnimUnit(AnimUnit * u, int y, int x, int dy, int dx, uint8_t h, uint8_t w, uint8_t * texture, uint8_t frames){
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->h = h;
    u->w = w;
    u->texture = texture;
    u->frames = frames;
    u->frame = 0;
}
void draw_AnimUnit(AnimUnit * u){
    screen_display_texture(u->y, u->x, u->h, u->w, u->texture + u->frame++ / 4 % u->frames * u->h * u->w);
}
