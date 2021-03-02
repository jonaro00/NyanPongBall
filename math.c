
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

int sign(float f){
    return f > 0 ? 1 : (f < 0 ? -1 : 0);
}

int ixor(float a, float b){
    return ((sign(a)>=0 && sign(b)<0) || (sign(a)<0 && sign(b)>=0)) ? 1 : -1;
}

int floorMod(int d, int m){
    int mod = d % m;
    if(m >= 0) return mod >= 0 ? mod : mod+m;
    else       return mod <  0 ? mod : mod+m;
}
