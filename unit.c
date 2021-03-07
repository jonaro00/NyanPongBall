
#include <stdint.h>
#include "game.h"

// Constructor for basic game unit.
void init_Unit(Unit *u, float y, float x, float dy, float dx, float ay, float ax, uint8_t h, uint8_t w, uint8_t *texture, int xdir){
    u->active = 1;
    u->alive = 1;
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->ay = ay;
    u->ax = ax;
    u->h = h;
    u->w = w;
    u->texture = texture;
    u->xdir = xdir;
}
// Update unit's velocity and position.
void move_Unit(Unit *u){
    if(!u->active) return;
    u->dx += u->ax;
    u->dy += u->ay;
    u->x += u->dx;
    u->y += u->dy;
}
// Draws unit on screen.
void draw_Unit(Unit *u){
    if(u->active)
        screen_display_texture(round(u->y), round(u->x), u->h, u->w, u->texture, u->xdir);
}

// Constructor for animated unit.
void init_AnimUnit(AnimUnit *u, float y, float x, float dy, float dx, float ay, float ax, uint8_t h, uint8_t w, uint8_t *texture, int xdir, uint8_t frames, uint8_t period){
    u->active = 1;
    u->alive = 1;
    u->y = y;
    u->x = x;
    u->dy = dy;
    u->dx = dx;
    u->ay = ay;
    u->ax = ax;
    u->h = h;
    u->w = w;
    u->texture = texture;
    u->xdir = xdir;
    u->frames = frames;
    u->frame = 0;
    u->period = period;
}
// Draws animated unit on screen. Increments it's frame counter.
void draw_AnimUnit(AnimUnit *u){
    if(u->active)
        screen_display_texture(round(u->y), round(u->x), u->h, u->w, u->texture + u->frame++/u->period % u->frames * u->h * u->w, u->xdir);
}

// Checks for rectangle overlap between two units.
uint8_t collides(Unit *u1, Unit *u2){
    // if one rectangle is above the other, there is no collision.
    if(u1->x >= u2->x+u2->w-1 || u2->x >= u1->x+u1->w-1) return 0;
    // if one rectangle is to the right of the other, there is no collision.
    if(u1->y >= u2->y+u2->h-1 || u2->y >= u1->y+u1->h-1) return 0;
    return 1;
}
// Checks if unit's bounding box is within the screen.
uint8_t unit_on_screen(Unit *u){
    if(is_on_screen(u->y,u->x) ||
       is_on_screen(u->y,u->x+u->w-1) ||
       is_on_screen(u->y+u->h-1,u->x) ||
       is_on_screen(u->y+u->h-1,u->x+u->w-1))
       return 1; // if any of the corners is on screen, the unit is.
    return 0;
}
