#include "game.h"
#include "../util/list.h"
#include "asteroid.h"
#include "../drivers/video.h"
#include "../kernel.h"
#include "../debug.h"
#include "../drivers/keyboard.h"
#include "../drivers/timer.h"
#include "player.h"
#include "../math.h"
#include "stars.h"
#include "map.h"

Map map;
vec camera_pos;
float asteroid_spawn_time = 50;
float border_pushing = 0.08f;
int32 border_width_cells = 2;
int32 asteroid_min_size = 8;
int32 asteroid_max_size = 22;

static uint8 isInCamera(vec pos, float object_size)
{
    return (pos.x > camera_pos.x - object_size / 2 && pos.x < camera_pos.x + object_size / 2 + screen_width) || (pos.y > camera_pos.y - object_size / 2 && pos.y < camera_pos.y + screen_height + object_size / 2);
}

static void _spawn_asteroid()
{
    int32 size = rand_int(asteroid_min_size, asteroid_max_size);

    float x;
    float y;
    do {
        x = rand_float(map.corner00_pos.x + border_width_cells * map.cell_size, map.corner11_pos.x - 1 - border_width_cells * map.cell_size);
        y = rand_float(map.corner00_pos.y + border_width_cells * map.cell_size, map.corner11_pos.y - 1 - border_width_cells * map.cell_size);
    } while (isInCamera((vec){ x, y }, size));
    vec pos = { x, y };

    float speed = rand_float(0.1, 1.0);
    float angle = rand_float(0.0, 2 * PI);
    vec velocity = { cos(angle) * speed, sin(angle) * speed };

    map_push_asteroid(&map, asteroid_new(pos, velocity, (vec){ 0, 0 }, size));
}

static void spawn_start_asteroids()
{
    for (int i = 0; i < 50; i++)
        _spawn_asteroid();
}

static void spawn_asteroids()
{
    _spawn_asteroid();
    add_event(spawn_asteroids, asteroid_spawn_time);
}

static void _draw_stars(int32 pos_x, int32 pos_y)
{
    int32 x_limit = screen_width + (pos_x > 0 ? -pos_x : 0);
    int32 y_limit = screen_height + (pos_y > 0 ? -pos_y : 0);
    int32 x_base = (pos_x < 0 ? -pos_x : 0);
    int32 y_base = (pos_y < 0 ? -pos_y : 0);

    for (uint32 y = y_base; y < y_limit; y++)
    {
        for (uint32 x = x_base; x < x_limit; x++)
        {
            uint8 star = starfield[x + y * screen_width];
            *((uint8*)double_buffer + x + pos_x + (y + pos_y) * screen_width) = star;
        }
    }
}

static void stars_graphics()
{
    _draw_stars((int32)stars_pos.x, (int32)stars_pos.y);
    _draw_stars((int32)stars_pos.x + screen_width, (int32)stars_pos.y);
    _draw_stars((int32)stars_pos.x, (int32)stars_pos.y + screen_height);
    _draw_stars((int32)stars_pos.x + screen_width, (int32)stars_pos.y + screen_height);
}

static void stars_logic()
{
    //stars_pos = vec_sum(vec_mul(player.velocity, -0.12), stars_pos);
    stars_pos = vec_mul(camera_pos, -0.12);
    if (stars_pos.x > 0.0)
        stars_pos.x -= screen_width;
    if (stars_pos.x < -(int32)screen_width)
        stars_pos.x += screen_width;
    if (stars_pos.y > 0.0)
        stars_pos.y -= screen_height;
    if (stars_pos.y < -(int32)screen_height)
        stars_pos.y += screen_height;
}

static void player_input()
{
    if (!key_none(SC_W))
    {
        player.velocity.x += cos(player.rotation_angle) * player.speed;
        player.velocity.y += sin(player.rotation_angle) * player.speed;
    }
    if (!key_none(SC_S))
    {
        player.velocity.x -= cos(player.rotation_angle) * player.speed;
        player.velocity.y -= sin(player.rotation_angle) * player.speed;
    }
    if (!key_none(SC_A))
        player.rotation_angle -= 0.09;
    if (!key_none(SC_D))
        player.rotation_angle += 0.09;
}

static void player_logic()
{
    player.pos.x += player.velocity.x;
    player.pos.y += player.velocity.y;

    camera_pos = vec_sub(player.pos, (vec){ screen_width / 2, screen_height / 2 });
    camera_pos.x = dmax(camera_pos.x, map.corner00_pos.x + border_width_cells * map.cell_size);
    camera_pos.y = dmax(camera_pos.y, map.corner00_pos.y + border_width_cells * map.cell_size);
    camera_pos.x = dmin(camera_pos.x, map.corner11_pos.x - border_width_cells * map.cell_size - (int32)screen_width);
    camera_pos.y = dmin(camera_pos.y, map.corner11_pos.y - border_width_cells * map.cell_size - (int32)screen_height);

    player.velocity.x *= 0.987;
    player.velocity.y *= 0.987;

    if (player.pos.x < map.corner00_pos.x + border_width_cells * map.cell_size)
        player.velocity.x += border_pushing;
    if (player.pos.y < map.corner00_pos.y + border_width_cells * map.cell_size)
        player.velocity.y += border_pushing;
    if (player.pos.x > map.corner11_pos.x - border_width_cells * map.cell_size)
        player.velocity.x -= border_pushing;
    if (player.pos.y > map.corner11_pos.y - border_width_cells * map.cell_size)
        player.velocity.y -= border_pushing;
}

static void player_graphics()
{
    vec player_pos = vec_sub(player.pos, camera_pos);
    float player_size = 7;
    double t = 2.09;
    vec point1 = vec_sum(player_pos, (vec){ cos(player.rotation_angle) * player_size, sin(player.rotation_angle) * player_size });
    vec point2 = vec_sum(player_pos, (vec){ cos(player.rotation_angle - t) * player_size / 2, sin(player.rotation_angle - t) * player_size / 2 });
    vec point3 = vec_sum(player_pos, (vec){ cos(player.rotation_angle + t) * player_size / 2, sin(player.rotation_angle + t) * player_size / 2 });
    drawLines(point1.x, point1.y, point2.x, point2.y, 0x0E);
    drawLines(point1.x, point1.y, point3.x, point3.y, 0x0E);
    drawLines(point3.x, point3.y, point2.x, point2.y, 0x0E);
}

static void iterate_asteroids_graphics()
{
    int32 start_x = (camera_pos.x - map.corner00_pos.x) / map.cell_size - 1;
    int32 start_y = (camera_pos.y - map.corner00_pos.y) / map.cell_size - 1;
    int32 end_x = (camera_pos.x + screen_width - map.corner00_pos.x) / map.cell_size + 2;
    int32 end_y = (camera_pos.y + screen_height - map.corner00_pos.y) / map.cell_size + 2;
    for (int cell_x = max(0, start_x); cell_x < min(end_x, map.side_size); cell_x++)
    {
        for (int cell_y = max(0, start_y); cell_y < min(end_y, map.side_size); cell_y++)
        {
            for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL; i = i->next)
            {
                Asteroid* a_data = (Asteroid*)i->data;
                //drawFillCircles(a_data->pos.x + camera_pos.x, a_data->pos.y + camera_pos.y, a_data->size, 3);
                if (!isInCamera(a_data->pos, a_data->size * ASTEROID_SIZE_MAX))
                    continue;

                vec first_pos = { a_data->pos.x - camera_pos.x + a_data->verticies[0].x, a_data->pos.y - camera_pos.y + a_data->verticies[0].y };
                vec next_pos = first_pos;
                for (int i = 1; i < a_data->verticies_count; i++)
                {
                    vec pos = { a_data->pos.x - camera_pos.x + a_data->verticies[i].x, a_data->pos.y - camera_pos.y + a_data->verticies[i].y };
                    drawLines(pos.x, pos.y, next_pos.x, next_pos.y, 6);
                    next_pos = pos;
                }
                drawLines(first_pos.x, first_pos.y, next_pos.x, next_pos.y, 6);
            }
        }
    }
}

static void iterate_asteroids_logic()
{
    for (int cell_x = 0; cell_x < map.side_size; cell_x++)
    {
        for (int cell_y = 0; cell_y < map.side_size; cell_y++)
        {
            for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL;)
            {
                Asteroid* a_data = (Asteroid*)i->data;
                a_data->pos = vec_sum(a_data->pos, a_data->velocity);
                a_data->velocity = vec_sum(a_data->velocity, a_data->acceleration);

                if (a_data->pos.x < map.corner00_pos.x + border_width_cells * map.cell_size)
                    a_data->velocity.x += border_pushing;
                if (a_data->pos.y < map.corner00_pos.y + border_width_cells * map.cell_size)
                    a_data->velocity.y += border_pushing;
                if (a_data->pos.x > map.corner11_pos.x - border_width_cells * map.cell_size)
                    a_data->velocity.x -= border_pushing;
                if (a_data->pos.y > map.corner11_pos.y - border_width_cells * map.cell_size)
                    a_data->velocity.y -= border_pushing;

                int32 c_x = a_data->pos.x - map.corner00_pos.x;
                c_x /= map.cell_size;
                int32 c_y = a_data->pos.y - map.corner00_pos.y;
                c_y /= map.cell_size;
                if (c_x != cell_x || c_y != cell_y)
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].asteroids, i);
                    //dbg_printf("Moving asteroid from %d %d to %d %d", );
                    map_push_asteroid(&map, a_data);
                    i = i_next;
                }
                else i = i->next;
            }
        }
    }
} 

void game_init() {
    player_constructor((vec){ 0, 0 }, (vec){ 0, 0 }, (vec) { 0, 0 }, 0, 0.023); 
    map_constructor(&map, (ivec){ 0, 0 }, 80, 20);
    init_stars();

    add_event(spawn_asteroids, asteroid_spawn_time);
    camera_pos.x = -(int32)screen_width / 2;
    camera_pos.y = -(int32)screen_height / 2;
    spawn_start_asteroids();
}

void game_input() {
    player_input();
    if (key_press(SC_ESCAPE)) run = 0;
}

void game_logic() {
    player_logic();
    iterate_asteroids_logic();
    stars_logic();
}

void game_graphics() {
    stars_graphics();
    iterate_asteroids_graphics();
    player_graphics();
}

void game_destruct() {
    map_destructor(&map);
    destruct_stars();
}