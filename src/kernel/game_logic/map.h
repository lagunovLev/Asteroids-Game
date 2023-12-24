#pragma once
#include "../util/ivec.h"
#include "../util/vec.h"
#include "../common.h"
#include "../util/list.h"
#include "asteroid.h"

typedef struct {
    List asteroids;
    //List bullets;
} Cell;

void cell_constructor(Cell* this);
void cell_destructor(Cell* this);
Cell* cell_new();
void cell_delete(Cell* this);

typedef struct {
    ivec corner00_pos;
    ivec corner01_pos;
    ivec corner10_pos;
    ivec corner11_pos;
    ivec center_pos;
    uint16 cell_size;
    uint16 side_size;
    Cell** cells;
} Map;

void map_constructor(Map* this, ivec center_pos, uint16 cell_size, uint16 side_size);
void map_destructor(Map* this);
void map_push_asteroid(Map* this, Asteroid* asteroid);
void map_push_asteroid_pos(Map* this, Asteroid* asteroid, ivec pos);
