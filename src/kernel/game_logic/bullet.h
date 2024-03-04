#pragma once
#include "../util/vec.h"
#include "../common.h"

typedef struct {
    vec pos;
    vec velocity;
    vec acceleration;
    uint8 moved;
} Bullet;

Bullet* bullet_new(vec pos, vec velocity, vec acceleration);
void bullet_delete(Bullet* this);
void bullet_constructor(Bullet* this, vec pos, vec velocity, vec acceleration);
void bullet_destructor(Bullet* this);