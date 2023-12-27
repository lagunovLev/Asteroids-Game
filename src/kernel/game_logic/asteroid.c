#include "asteroid.h"
#include "../math.h"
 
Asteroid* asteroid_new(vec pos, vec velocity, vec acceleration, uint16 size, uint8 strong)
{
    Asteroid* asteroid = malloc(sizeof(Asteroid));
    asteroid_constructor(asteroid, pos, velocity, acceleration, size, strong);
    return asteroid;
}

void asteroid_delete(Asteroid* this)
{
    asteroid_destructor(this);
    free(this); 
}

void asteroid_constructor(Asteroid* this, vec pos, vec velocity, vec acceleration, uint16 size, uint8 strong)
{
    this->pos = pos;
    this->velocity = velocity;
    this->acceleration = acceleration;
    this->strong = strong;
    this->size = size;
    this->damaged = 0;
    this->verticies_count = size * PI / 3;
    this->angle = 0;
    this->rotation_speed = rand_float(-0.02, 0.02);
    if (this->verticies_count % 2 != 0) this->verticies_count++;
    this->verticies = malloc(sizeof(float) * this->verticies_count);
    float average_size = 0;

    for (int i = 0; i < this->verticies_count; i += 2)
    {
        this->verticies[i] = rand_float(ASTEROID_SIZE_MIN, ASTEROID_SIZE_MAX);
        average_size += this->verticies[i];
        this->verticies[i] *= this->size;
    }
    average_size /= this->verticies_count / 2;
    for (int i = 1; i < this->verticies_count - 1; i += 2)
    {
        this->verticies[i] = (this->verticies[i-1] + this->verticies[i+1]) / 2;
    }
    this->verticies[this->verticies_count - 1] = (this->verticies[this->verticies_count - 2] + this->verticies[0]) / 2;

    this->size *= average_size;
    this->size *= 1.2;
    this->health = PI * this->size * this->size;
    if (this->strong)
        this->health *= 2;
}

void asteroid_destructor(Asteroid* this)
{
    free(this->verticies);
}
