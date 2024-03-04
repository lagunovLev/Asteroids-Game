#pragma once 
#include "../common.h"
#include "../util/vec.h"

#define ASTEROID_SIZE_MIN 0.6
#define ASTEROID_SIZE_MAX 1.4

typedef struct {
    vec position_current;
    vec position_old;
    vec acceleration;
    uint16 size;
    uint16 verticies_count;
    float* verticies;
    uint8 damaged;
    float health;
    float angle;
    float rotation_speed;
    uint8 strong;
    uint8 moved;
} Asteroid;

Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size, uint8 strong);
void asteroid_delete(Asteroid* this);
void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size, uint8 strong);
void asteroid_destructor(Asteroid* this);
vec asteroid_velocity(Asteroid* this);