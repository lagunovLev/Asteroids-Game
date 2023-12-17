#pragma once 
#include "../common.h"
#include "../util/vec.h"

#define ASTEROID_VERTICIES 24

typedef struct {
    vec pos;
    vec velocity;
    vec acceleration;
    uint16 size;
    vec verticies[ASTEROID_VERTICIES];
} Asteroid;

Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_delete(Asteroid* this);
void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_destructor(Asteroid* this);