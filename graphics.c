
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
void quicksleep(int cyc){
    int i;
    for(i = cyc; i > 0; i--);
}

/* Converts a number to hexadecimal ASCII digits. */
void num32asc(char *s, int n){
    int i;
    for(i = 28; i >= 0; i -= 4)
        *s++ = "0123456789ABCDEF"[(n >> i) & 15];
}

#define ITOA_BUFSIZ (24)
char *itoaconv(int num){
    register int i, sign;
    static char itoa_buffer[ITOA_BUFSIZ];
    static const char maxneg[] = "-2147483648";

    itoa_buffer[ITOA_BUFSIZ - 1] = 0; /* Insert the end-of-string marker. */
    sign = num;                       /* Save sign. */
    if (num < 0 && num - 1 > 0){      /* Check for most negative integer */
        for (i = 0; i < sizeof(maxneg); i++)
            itoa_buffer[i + 1] = maxneg[i];
        i = 0;
    }
    else{
        if (num < 0)
            num = -num;      /* Make number positive. */
        i = ITOA_BUFSIZ - 2; /* Location for first ASCII digit. */
        do{
            itoa_buffer[i] = num % 10 + '0'; /* Insert next digit. */
            num /= 10;                       /* Remove digit from number. */
            i--;                             /* Move index to next empty position. */
        } while (num > 0);
        if (sign < 0){
            itoa_buffer[i] = '-';
            i--;
        }
    }
    /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
    return &itoa_buffer[i + 1];
}

uint8_t spi_send_recv(uint8_t data) {
    while(!(SPI2STAT & 8));
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

void screen_fill(uint8_t p){
    uint8_t y, x;
    for(y = 0; y < SCREEN_HEIGHT; y++)
        for(x = 0; x < SCREEN_WIDTH; x++)
            screen[y][x] = p;
}

int is_on_screen(int y, int x){
    return (y >= 0 && y < SCREEN_HEIGHT && x >= 0 && x < SCREEN_WIDTH);
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
    screen_fill(0);
}

uint8_t screen_get_strip(uint8_t y, uint8_t x){
    uint8_t k, b = 0;
    for(k = 0; k < 8; k++)
        b |= screen[y+k][x] << k;
    return b;
}

void screen_set_strip(int y, int x, uint8_t b){
    int k, sy;
    for(k = 0; k < 8; k++){
        sy = y+k;
        if(!is_on_screen(sy, x)) continue;
        screen[sy][x] = (b >> k) & 0x1;
    }
}

void screen_display_string(int y, int x, char *s){
    int i, ro = 0, co = 0;
    char c;
    uint8_t pcol;
    for(i = 0; i < MAX_SCORES*13; i++){
        c = *s++;
        if(c == 0) break;
        if(c == '\n'){ // insert newline, offset rest of string
            ro++;
            co=i+1;
            continue;
        }
        for(pcol = 0; pcol < 6; pcol++)
            screen_set_strip(y+ro*8, x+(i-co)*6+pcol, font[c*6 + pcol]);
    }
}

void screen_display_texture(int y, int x, uint8_t h, uint8_t w, uint8_t *t, int xdir){
    int i, j, sy, sx, jj;
    uint8_t p;
    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            jj = xdir > 0 ? j : w-1-j;
            sy = y+i; sx = x+jj;
            if(!is_on_screen(sy, sx)) continue;
            p = *(t+i*w+j);
            if(p == 2) continue;
            screen[sy][sx] = p;
        }
    }
}

void screen_draw_box(int y, int x, uint8_t h, uint8_t w, uint8_t p){
    int i, j, sy, sx;
    for(i = 0; i < h; i++){
        for(j = 0; j < w; j++){
            sy = y+i; sx = x+j;
            if(!is_on_screen(sy, sx)) continue;
            screen[sy][sx] = p;
        }
    }
}

char debug_row1[] = "A=76543210";
char debug_row2[] = "D=76543210";
void print_debug(const volatile int* addr){
    num32asc(debug_row1+2, (int)addr);
    num32asc(debug_row2+2, *addr);
    screen_display_string(8, 60, debug_row1);
    screen_display_string(16, 60, debug_row2);
}
