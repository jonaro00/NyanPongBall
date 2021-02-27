
#include <stdint.h>
#include "game.h"

int round(float f){
    return f + (f >= 0 ? 0.5 : -0.5);
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