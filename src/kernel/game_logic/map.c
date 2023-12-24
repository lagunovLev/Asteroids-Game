#include "map.h"
#include "asteroid.h"
#include "../debug.h"

void cell_constructor(Cell* this) {
    list_constructor(&this->asteroids);
    //list_constructor(&this->bullets);
}

void cell_destructor(Cell* this) {
    list_destructor(&this->asteroids, asteroid_delete);
    //list_destructor(&this->bullets);
}

Cell* cell_new() {
    Cell* cell = malloc(sizeof(Cell));
    cell_constructor(cell);
    return cell;
}

void cell_delete(Cell* this) {
    cell_destructor(this);
    free(this);
}


void map_constructor(Map* this, ivec center_pos, uint16 cell_size, uint16 side_size)
{
    this->center_pos = center_pos;
    this->cell_size = cell_size;
    this->side_size = side_size;
    uint16 side_size_in_pixels = cell_size * side_size;
    this->corner00_pos = ivec_sub(center_pos, ivec_div((ivec){ side_size_in_pixels, side_size_in_pixels }, 2));
    this->corner11_pos = ivec_sum(center_pos, ivec_div((ivec){ side_size_in_pixels, side_size_in_pixels }, 2));
    this->corner01_pos = ivec_sub(this->corner11_pos, (ivec){ side_size_in_pixels, 0 });
    this->corner10_pos = ivec_sub(this->corner11_pos, (ivec){ 0, side_size_in_pixels });

    this->cells = malloc(sizeof(Cell*) * side_size);
    for (int i = 0; i < side_size; i++)
    {
        this->cells[i] = malloc(sizeof(Cell) * side_size);
        for (int j = 0; j < side_size; j++)
        {
            cell_constructor(&this->cells[i][j]);
        }
    }
}

void map_destructor(Map* this)
{
    for (int i = 0; i < this->side_size; i++)
    {
        for (int j = 0; j < this->side_size; j++)
        {
            cell_destructor(&this->cells[i][j]);
        }
        free(this->cells[i]);
    }
    free(this->cells);
}

void map_push_asteroid(Map* this, Asteroid* asteroid)
{
    if (asteroid->pos.x < this->corner00_pos.x || asteroid->pos.x >= this->corner11_pos.x || 
        asteroid->pos.y < this->corner00_pos.y || asteroid->pos.y >= this->corner11_pos.y)
    {
        asteroid_delete(asteroid);
        return;
    }

    ivec pos = { asteroid->pos.x, asteroid->pos.y };
    pos = ivec_sub(pos, this->corner00_pos);
    pos = ivec_div(pos, this->cell_size);
    list_push_back(&this->cells[pos.x][pos.y].asteroids, asteroid);
    //dbg_printf("Spawned asteroid %d %d\n", pos.x, pos.y);
}

void map_push_asteroid_pos(Map* this, Asteroid* asteroid, ivec pos)
{
    list_push_back(&this->cells[pos.x][pos.y].asteroids, asteroid);
}