
#include <stdint.h>
#include "game.h"

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

int indexOf(char c, char *str){
    int i = 0;
    while(*str != 0){
        if(*str == c) return i;
        i++; str++;
    }
    return -1;
}

void insert(char * src, char * target, int i, uint8_t nullend){
    while(*src != 0){
        target[i] = *src;
        src++; i++;
    }
    if(nullend) target[i] = 0;
}