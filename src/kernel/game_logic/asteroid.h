#pragma once 
#include "../common.h"
#include "../util/vec.h"

typedef struct {
    vec pos;
    vec velocity;
    vec acceleration;
    uint16 size;
} Asteroid;

Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_delete(Asteroid* this);
void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_destructor(Asteroid* this);