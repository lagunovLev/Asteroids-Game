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

List asteroids;
float asteroid_spawn_time = 10;
vec camera_pos;
int32 simulation_radius = 1500;
int32 spawn_radius_big = 1000;
int32 spawn_radius_small = 25;

int32 asteroid_min_size = 5;
int32 asteroid_max_size = 25;

int32 partition_size = 100;

static void _spawn_asteroid()
{
    int32 size = rand_int(asteroid_min_size, asteroid_max_size);

    float x = rand_int(0, 1) ? rand_float(-spawn_radius_big + player.pos.x, -spawn_radius_small + player.pos.x - (int32)screen_width / 2) : rand_float(spawn_radius_small + player.pos.x + screen_width / 2, spawn_radius_big + player.pos.x);
    float y = rand_int(0, 1) ? rand_float(-spawn_radius_big + player.pos.y, -spawn_radius_small + player.pos.y - (int32)screen_height / 2) : rand_float(spawn_radius_small + player.pos.y + screen_height / 2, spawn_radius_big + player.pos.y);
    vec pos = { x, y };

    float speed = rand_float(0.1, 1.0);
    float angle = rand_float(0.0, 2 * PI);
    vec velocity = { cos(angle) * speed, sin(angle) * speed };

    list_push_back(&asteroids, asteroid_new(pos, velocity, (vec){ 0, 0 }, size));
}

static void spawn_start_asteroids()
{
    for (int i = 0; i < 200; i++)
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
    stars_pos = vec_sum(vec_mul(player.velocity, -0.12), stars_pos);
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

    camera_pos.x -= player.velocity.x;
    camera_pos.y -= player.velocity.y;

    player.velocity.x *= 0.987;
    player.velocity.y *= 0.987;
}

static void player_graphics()
{
    vec player_pos = vec_sum(player.pos, camera_pos);
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
    for (list_elem* i = asteroids.begin; i != NULL; i = i->next)
    {
        Asteroid* a_data = (Asteroid*)i->data;
        //drawFillCircles(a_data->pos.x + camera_pos.x, a_data->pos.y + camera_pos.y, a_data->size, 6);

        vec first_pos = { a_data->pos.x + camera_pos.x + a_data->verticies[0].x, a_data->pos.y + camera_pos.y + a_data->verticies[0].y };
        vec next_pos = first_pos;
        for (int i = 1; i < ASTEROID_VERTICIES; i++)
        {
            vec pos = { a_data->pos.x + camera_pos.x + a_data->verticies[i].x, a_data->pos.y + camera_pos.y + a_data->verticies[i].y };
            drawLines(pos.x, pos.y, next_pos.x, next_pos.y, 6);
            next_pos = pos;
        }
        drawLines(first_pos.x, first_pos.y, next_pos.x, next_pos.y, 6);
    }
}

static void iterate_asteroids_logic()
{
    for (list_elem* i = asteroids.begin; i != NULL;)
    {
        Asteroid* a_data = (Asteroid*)i->data;
        a_data->pos = vec_sum(a_data->pos, a_data->velocity);
        a_data->velocity = vec_sum(a_data->velocity, a_data->acceleration);

        float player_distance = vec_distance(a_data->pos, player.pos);
        if (player_distance > simulation_radius)
        {
            list_elem* i_next = i->next;
            list_remove_by_elem(&asteroids, i);
            asteroid_delete(a_data);
            i = i_next;
        }
        else i = i->next;
    }
} 

void game_init() {
    player_constructor((vec){ 0, 0 }, (vec){ 0, 0 }, (vec) { 0, 0 }, 0, 0.023);
    init_stars();

    list_constructor(&asteroids);
    add_event(spawn_asteroids, asteroid_spawn_time);
    camera_pos.x = screen_width / 2;
    camera_pos.y = screen_height / 2;
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
    list_destructor(&asteroids, asteroid_delete);
    destruct_stars();
}