#include "asteroid.h"

Asteroid* asteroid_new(int16 x, int16 y)
{
    Asteroid* asteroid = malloc(sizeof(Asteroid));
    asteroid_constructor(asteroid, x, y);
    return asteroid;
}

void asteroid_delete(Asteroid* this)
{
    asteroid_destructor(this);
    free(this);
}

void asteroid_constructor(Asteroid* this, int16 x, int16 y)
{
    this->x = x;
    this->y = y;
}

void asteroid_destructor(Asteroid* this)
{
    
}
