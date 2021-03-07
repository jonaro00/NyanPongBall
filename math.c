
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

// Returns f if within min/max boundraries, else returns the broken boundrary.
float bound(float min, float f, float max){
    return (f < min ? min : (f > max ? max : f));
}

float avg(float a, float b){
    return (a+b)/2;
}

float abs(float f){
    return f >= 0 ? f : -f;
}

// Returns 1/0/-1 based on sign of f.
int sign(float f){
    return f > 0 ? 1 : (f < 0 ? -1 : 0);
}

// Like XOR, but for positive/negative integers instead of 1/0.
// Returns 1 or -1.
int ixor(float a, float b){
    return ((sign(a)>=0 && sign(b)<0) || (sign(a)<0 && sign(b)>=0)) ? 1 : -1;
}

int floorMod(int d, int m){
    int mod = d % m;
    if(m >= 0) return mod >= 0 ? mod : mod+m;
    else       return mod <  0 ? mod : mod+m;
}

float pow(float a, int b){
    if(b < 0) return 0;
    if(b == 0) return 1;
    if(b == 1) return a;
    int i;
    float c = a;
    for(i = 1; i < b; i++)
        c *= a;
    return c;
}

// Pseudo-random generator

uint32_t seed = 0;
// Returns 32 pseudo-random bits.
uint32_t xorshifter(){
    if(!seed) seed = tick_start;
    int i;
    for(i = 0; i < 64; i++){
        seed ^= seed >> 16;
        seed ^= seed << 25;
        seed ^= seed >> 7;
    }
    return seed;
}
// Returns pseudo-random float value in range [0, 1).
float random(){
    uint32_t r = xorshifter();
    int i;
    float f = 0;
    for(i = 1; i < 16; i++)
        f += ((r >> i) & 1) / pow(2, i);
    return f;
}
