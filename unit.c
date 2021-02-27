
#include <stdint.h>
#include "game.h"

void init_Unit(Unit * u, float y, float x, float dy, float dx, uint8_t h, uint8_t w, uint8_t * texture, int xdir){
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->h = h;
    u->w = w;
    u->texture = texture;
    u->xdir = xdir;
}
void draw_Unit(Unit * u){
    screen_display_texture(round(u->y), round(u->x), u->h, u->w, u->texture, u->xdir);
}

void init_AnimUnit(AnimUnit * u, float y, float x, float dy, float dx, uint8_t h, uint8_t w, uint8_t * texture, int xdir, uint8_t frames){
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->h = h;
    u->w = w;
    u->texture = texture;
    u->xdir = xdir;
    u->frames = frames;
    u->frame = 0;
}
void draw_AnimUnit(AnimUnit * u){
    screen_display_texture(round(u->y), round(u->x), u->h, u->w, u->texture + u->frame++ / ANIM_SPEED % u->frames * u->h * u->w, u->xdir);
}
