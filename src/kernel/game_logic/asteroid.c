#include "asteroid.h"
 
Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size)
{
    Asteroid* asteroid = malloc(sizeof(Asteroid));
    asteroid_constructor(asteroid, pos, velocity, acceleration, size);
    return asteroid;
}

void asteroid_delete(Asteroid* this)
{
    asteroid_destructor(this);
    free(this); 
}

void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size)
{
    this->pos = pos;
    this->velocity = velocity;
    this->acceleration = acceleration;
    this->size = size;
}

void asteroid_destructor(Asteroid* this)
{

}
