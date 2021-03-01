
#include <stdint.h>
#include "game.h"

int round(float f){
    return f + (f >= 0 ? 0.5 : -0.5);
}

float min(float a, float b){
    return a < b ? a : b;
}

float max(float a, float b){
    return a > b ? a : b;
}

float bound(float min, float f, float max){
    return (f < min ? min : (f > max ? max : f));
}

float avg(float a, float b){
    return (a+b)/2;
}

float abs(float f){
    return f >= 0 ? f : -f;
}

int floorMod(int d, int m){
    int mod = d % m;
    if(m >= 0) return mod >= 0 ? mod : mod+m;
    else       return mod <  0 ? mod : mod+m;
}

int indexOf(char c, char *str){
    int i = 0;
    while(*str != 0){
        if(*str == c) return i;
        i++; str++;
    }
    return -1;
}
