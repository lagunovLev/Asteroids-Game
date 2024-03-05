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
    this->position_current = pos;
    this->position_old = vec_sub(pos, velocity);
    this->acceleration = acceleration;
    this->strong = strong;
    this->size = size;
    this->damaged = 0;
    //this->verticies_count = size * PI / 3;
    this->verticies_count = 16;
    this->angle = 0;
    this->rotation_speed = rand_float(-0.03, 0.03);
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
    this->health = 2.5 * this->size * this->size;
    if (this->strong)
        this->health *= 2;
    this->moved = 0;
}

void asteroid_destructor(Asteroid* this)
{
    free(this->verticies);
}

vec asteroid_velocity(Asteroid* this)
{
    return vec_sub(this->position_current, this->position_old);
}

float asteroid_mass(Asteroid* this)
{
    float volume = this->size * this->size * PI;
    float mass = volume;
    if (this->strong)
        mass *= 2;
    return mass;
}