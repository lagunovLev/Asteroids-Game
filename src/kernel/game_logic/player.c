#include "player.h"

Player player;

void player_constructor(vec pos, vec velocity, vec acceleration, float rotation_angle, float speed)
{
    player.position_current = pos;
    player.position_old = vec_sub(pos, velocity);
    player.acceleration = acceleration;
    player.rotation_angle = rotation_angle;
    player.speed = speed;
}

vec player_velocity(Player* this)
{
    return vec_sub(this->position_current, this->position_old);
}