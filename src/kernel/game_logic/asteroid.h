#pragma once 
#include "../common.h"
#include "../util/vec.h"

#define ASTEROID_SIZE_MIN 0.6
#define ASTEROID_SIZE_MAX 1.4

typedef struct {
    vec pos;
    vec velocity;
    vec acceleration;
    uint16 size;
    uint16 verticies_count;
    float* verticies;
    uint8 damaged;
    float health;
    float angle;
    float rotation_speed;
} Asteroid;

Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_delete(Asteroid* this);
void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size);
void asteroid_destructor(Asteroid* this);