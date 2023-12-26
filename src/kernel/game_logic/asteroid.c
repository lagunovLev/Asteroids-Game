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
    this->damaged = 0;
    this->verticies_count = size * PI / 3;
    this->angle = 0;
    this->rotation_speed = rand_float(-0.02, 0.02);
    if (this->verticies_count % 2 != 0) this->verticies_count++;
    this->verticies = malloc(sizeof(float) * this->verticies_count);
    float average_size = 0;
    //float* t = malloc(sizeof(float) * this->verticies_count);

    //float step = 2 * PI / this->verticies_count;
    //for (int i = 0; i < this->verticies_count; i += 2)
    //{
    //    float r = rand_float(ASTEROID_SIZE_MIN, ASTEROID_SIZE_MAX);
    //    average_size += r;
    //    t[i] = r;
    //    vec vert_pos = { cos(step * i) * size * r, sin(step * i) * size * r };
    //    this->verticies[i] = vert_pos;
    //}
    //average_size /= this->verticies_count / 2;

    //for (int i = 1; i < this->verticies_count - 1; i += 2)
    //{
    //    float r = (t[i-1] + t[i+1]) / 2;
    //    vec vert_pos = { cos(step * i) * size * r, sin(step * i) * size * r };
    //    this->verticies[i] = vert_pos;
    //}
    //float r = (t[this->verticies_count - 2] + t[0]) / 2;
    //vec vert_pos = { cos(step * (this->verticies_count - 1)) * size * r, sin(step * (this->verticies_count - 1)) * size * r };
    //this->verticies[this->verticies_count - 1] = vert_pos;
    //free(t);
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
    this->size *= 1.1;
    this->health = PI * this->size * this->size;
}

void asteroid_destructor(Asteroid* this)
{
    free(this->verticies);
}
