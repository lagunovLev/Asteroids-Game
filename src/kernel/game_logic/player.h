#pragma once
#include "../common.h"
#include "../util/vec.h"

typedef struct {
    vec position_current;
    vec position_old;
    vec acceleration;
    float rotation_angle;
    float speed;
} Player;

extern Player player;

void player_constructor(vec pos, vec velocity, vec acceleration, float rotation_angle, float speed);
vec player_velocity(Player* this);