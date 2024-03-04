#include "bullet.h"

Bullet* bullet_new(vec pos, vec velocity, vec acceleration) {
    Bullet* b = malloc(sizeof(Bullet));
    bullet_constructor(b, pos, velocity, acceleration);
    return b;
}

void bullet_delete(Bullet* this) {
    bullet_destructor(this);
    free(this);
}

void bullet_constructor(Bullet* this, vec pos, vec velocity, vec acceleration) {
    this->pos = pos;
    this->velocity = velocity;
    this->acceleration = acceleration;
    this->moved = 0;
}

void bullet_destructor(Bullet* this) {

}