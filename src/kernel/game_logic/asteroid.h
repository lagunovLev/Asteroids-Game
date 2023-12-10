#pragma once 
#include "../common.h"

typedef struct {
    int16 x;
    int16 y;
} Asteroid;

Asteroid* asteroid_new(int16 x, int16 y);
void asteroid_delete(Asteroid* this);
void asteroid_constructor(Asteroid* this, int16 x, int16 y);
void asteroid_destructor(Asteroid* this);