#include "asteroid.h"
#include "../math.h"
 
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

    float t[ASTEROID_VERTICIES];

    float step = 2 * PI / ASTEROID_VERTICIES;
    for (int i = 0; i < ASTEROID_VERTICIES - 1; i += 2)
    {
        float r = rand_float(0.6, 1.4);
        t[i] = r;
        vec vert_pos = { cos(step * i) * size * r, sin(step * i) * size * r };
        this->verticies[i] = vert_pos;
    }

    for (int i = 1; i < ASTEROID_VERTICIES; i += 2)
    {
        float r = (t[i-1] + t[i+1]) / 2;
        vec vert_pos = { cos(step * i) * size * r, sin(step * i) * size * r };
        this->verticies[i] = vert_pos;
    }
}

void asteroid_destructor(Asteroid* this)
{

}
