
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
    int i;
    for(i = cyc; i > 0; i--);
}

/* Converts a number to hexadecimal ASCII digits. */
void num32asc( char * s, int n )
{
    int i;
    for( i = 28; i >= 0; i -= 4 )
        *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 0x08));
    SPI2BUF = data;
    while(!(SPI2STAT & 1));
    return SPI2BUF;
}

void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}

uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];

void screen_reset(){
    int y, x;
    for (y = 0; y < SCREEN_HEIGHT; y++)
        for (x = 0; x < SCREEN_WIDTH; x++)
            screen[y][x] = 0;
}

void screen_render(){
    uint8_t i, x;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(x = 0; x < SCREEN_WIDTH; x++) {
            spi_send_recv(screen_get_strip(i*8, x));
        }
    }
    screen_reset();
}

uint8_t screen_get_strip(uint8_t y, uint8_t x){
    uint8_t k, b = 0;
    for(k = 0; k < 8; k++){
        b |= (screen[y+k][x] << k);
    }
    return b;
}

void screen_set_strip(uint8_t y, uint8_t x, uint8_t b){
    uint8_t k, sy, sx;
    for(k = 0; k < 8; k++){
        sy = y+k; sx = x;
        if(sy < 0 || sy >= SCREEN_HEIGHT || sx < 0 || sx >= SCREEN_WIDTH)
            continue;
        screen[sy][sx] = (b >> k) & 0x1;
    }
}

void screen_display_string(uint8_t y, uint8_t x, char *s){
    uint8_t c, k;
    for(c = 0; c < SCREEN_WIDTH / 6; c++){
        if(*s == 0) break;
        for(k = 1; k < 7; k++)
            screen_set_strip(y, x+c*6+k, font[*s*8 + k]);
        s++;
    }
}

void screen_display_texture(uint8_t y, uint8_t x, uint8_t h, uint8_t w, uint8_t *t){
    int i, j;
    uint8_t p, sy, sx;
    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            sy = y+i; sx = x+j;
            if(sy < 0 || sy >= SCREEN_HEIGHT || sx < 0 || sx >= SCREEN_WIDTH)
                continue;
            p = *(t+i*w+j);
            if(p == 2)
                continue;
            screen[sy][sx] = p;
        }
    }
}

void screen_draw_box(uint8_t y, uint8_t x, uint8_t h, uint8_t w, uint8_t p){
    uint8_t i, j, sy, sx;
    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            sy = y+i; sx = x+j;
            if(sy < 0 || sy >= SCREEN_HEIGHT || sx < 0 || sx >= SCREEN_WIDTH)
                continue;
            screen[sy][sx] = p;
        }
    }
}

char debug_row1[] = "A76543210";
char debug_row2[] = "D76543210";
void print_debug(const volatile int* addr){
    num32asc(debug_row1+1, (int)addr);
    num32asc(debug_row2+1, *addr);
    screen_display_string(0, 56, debug_row1);
    screen_display_string(8, 56, debug_row2);
}
