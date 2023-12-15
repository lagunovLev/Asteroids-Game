#include "player.h"

Player player;

void player_constructor(vec pos, vec velocity, vec acceleration, float rotation_angle, float speed)
{
    player.pos = pos;
    player.velocity = velocity;
    player.acceleration = acceleration;
    player.rotation_angle = rotation_angle;
    player.speed = speed;
}