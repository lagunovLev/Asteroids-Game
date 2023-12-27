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
#include "heart.h"
#include "gameover.h"

Map map;
vec camera_pos;
float asteroid_spawn_time;
float border_pushing;
int32 border_width_cells;
int32 asteroid_min_size;
int32 asteroid_max_size;
int8 player_health;
float player_size;
uint8 is_player_vulnerable;
int32 player_damage_blinking_freq;
int32 player_invulnerable_duration;
uint32 player_color;
uint32 ui_color;
int32 score;
uint32 shoot_cooldown;
uint8 reloading;
float strong_asteroid_chance;
float bullet_speed;
float bullet_damage;

int32 increase_difficulty_time1;
int32 increase_difficulty_time2;
int32 increase_difficulty_time3;
int32 increase_difficulty_time4;

static void init_values()
{
    asteroid_spawn_time = 120;
    border_pushing = 0.08f;
    border_width_cells = 2;
    asteroid_min_size = 5;
    asteroid_max_size = 18;
    player_health = 3;
    player_size = 6;
    is_player_vulnerable = 1;
    player_damage_blinking_freq = 8;
    player_invulnerable_duration = 120;
    player_color = 0x0E;
    ui_color = 0x5F;
    score = 0;
    shoot_cooldown = 8;
    reloading = 0;
    strong_asteroid_chance = 0;
    bullet_speed = 2.5;
    bullet_damage = 100;

    increase_difficulty_time1 = 2000;
    increase_difficulty_time2 = 3500;
    increase_difficulty_time3 = 5000;
    increase_difficulty_time4 = 6500;
}

static void increase_difficulty4()
{
    asteroid_spawn_time = 50;
    strong_asteroid_chance = 1;
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty3()
{
    asteroid_spawn_time = 60;
    strong_asteroid_chance = 0.7;
    add_event(increase_difficulty4, increase_difficulty_time4);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty2()
{
    asteroid_spawn_time = 75;
    strong_asteroid_chance = 0.4;
    add_event(increase_difficulty3, increase_difficulty_time3);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty1()
{
    asteroid_spawn_time = 100;
    add_event(increase_difficulty2, increase_difficulty_time2);
    dbg_printf("Increased difficulty!\n");
}

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
    uint8 strong = rand_float(0, 1) < strong_asteroid_chance ? 1 : 0; 

    float speed = rand_float(0.1, 0.8);
    float angle = rand_float(0.0, 2 * PI);
    vec velocity = { cos(angle) * speed, sin(angle) * speed };

    map_push_asteroid(&map, asteroid_new(pos, velocity, (vec){ 0, 0 }, size, strong));
}

static void player_vulnerable_event1();

static void player_vulnerable_event2()
{
    if (!is_player_vulnerable)
    {
        player_color = 0x0E;
        ui_color = 0x5F;
        add_event(player_vulnerable_event1, player_damage_blinking_freq);
    }
}

static void player_vulnerable_event1()
{
    if (!is_player_vulnerable)
    {
        player_color = 0x28;
        ui_color = 0x28;
        add_event(player_vulnerable_event2, player_damage_blinking_freq);
    }
}

static void player_invulnerable_end()
{
    player_color = 0x0E;
    ui_color = 0x5F;
    is_player_vulnerable = 1;
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

static void reloading_end()
{
    reloading = 0;
}

static void shoot()
{
    float a = player.rotation_angle + rand_float(-0.04, 0.04);
    float p_x = player.pos.x + cos(player.rotation_angle) * player_size;
    float p_y = player.pos.y + sin(player.rotation_angle) * player_size;
    float v_x = cos(a) * bullet_speed;
    float v_y = sin(a) * bullet_speed;
    map_push_bullet(&map, bullet_new((vec){ p_x, p_y }, (vec){ v_x, v_y }, (vec){ 0, 0 }));
    add_event(reloading_end, shoot_cooldown);
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
    if (reloading) return;
    if (!key_none(SC_SPACE))
    {
        reloading = 1;
        shoot();
    }
}

static void check_player_collisions(int32 cell_x, int32 cell_y)
{
    for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL; i = i->next)
    {
        Asteroid* a = i->data;
        float distance = vec_distance(a->pos, player.pos);
        if (distance <= player_size / 2 + a->size)
        {
            player_health--;
            if (player_health <= 0)
                run_current_scene = 0;
            is_player_vulnerable = 0;
            player_vulnerable_event1();
            add_event(player_invulnerable_end, player_invulnerable_duration);
        }
    }
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

    if (is_player_vulnerable) {
        int32 cell_player_x = (player.pos.x - map.corner00_pos.x) / map.cell_size;
        int32 cell_player_y = (player.pos.y - map.corner00_pos.y) / map.cell_size;

        check_player_collisions(cell_player_x, cell_player_y);
        uint8 left = 0, right = 0, top = 0, bottom = 0;
        float a_size = asteroid_max_size * ASTEROID_SIZE_MAX;
        if (player.pos.x - map.corner00_pos.x - map.cell_size * cell_player_x <= a_size)
        {
            left = 1;
            check_player_collisions(cell_player_x - 1, cell_player_y);
        }
        if (player.pos.y - map.corner00_pos.y - map.cell_size * cell_player_y <= a_size)
        {
            top = 1;
            check_player_collisions(cell_player_x, cell_player_y - 1);
        }
        if (map.corner00_pos.x + map.cell_size * (cell_player_x + 1) - player.pos.x <= a_size)
        {
            right = 1;
            check_player_collisions(cell_player_x + 1, cell_player_y);
        }
        if (map.corner00_pos.y + map.cell_size * (cell_player_y + 1) - player.pos.y <= a_size)
        {
            bottom = 1;
            check_player_collisions(cell_player_x, cell_player_y + 1);
        }
        if (left && top)
            check_player_collisions(cell_player_x - 1, cell_player_y - 1);
        if (left && bottom)
            check_player_collisions(cell_player_x - 1, cell_player_y + 1);
        if (right && top)
            check_player_collisions(cell_player_x + 1, cell_player_y - 1);
        if (right && bottom)
            check_player_collisions(cell_player_x + 1, cell_player_y + 1);
    }
}

static void player_graphics()
{
    vec player_pos = vec_sub(player.pos, camera_pos);
    double t = 2.09;
    vec point1 = vec_sum(player_pos, (vec){ cos(player.rotation_angle) * player_size, sin(player.rotation_angle) * player_size });
    vec point2 = vec_sum(player_pos, (vec){ cos(player.rotation_angle - t) * player_size / 2, sin(player.rotation_angle - t) * player_size / 2 });
    vec point3 = vec_sum(player_pos, (vec){ cos(player.rotation_angle + t) * player_size / 2, sin(player.rotation_angle + t) * player_size / 2 });
    drawLines(point1.x, point1.y, point2.x, point2.y, player_color);
    drawLines(point1.x, point1.y, point3.x, point3.y, player_color);
    drawLines(point3.x, point3.y, point2.x, point2.y, player_color);
}

static void iterate_cells_graphics()
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
                uint8 color;
                if (a_data->damaged)
                {
                    color = 0x0F;
                    a_data->damaged = 0;
                }
                else if (a_data->strong)
                    color = 0x18;
                else 
                    color = 6;

                if (!isInCamera(a_data->pos, a_data->size * ASTEROID_SIZE_MAX))
                    continue;

                float step = 2 * PI / a_data->verticies_count;
                vec vert_temp_pos = { cos(a_data->angle) * a_data->verticies[0], sin(a_data->angle) * a_data->verticies[0] };
                vert_temp_pos = vec_sum(a_data->pos, vert_temp_pos);
                vert_temp_pos = vec_sub(vert_temp_pos, camera_pos);
                vec first_pos = vert_temp_pos;
                for (int i = 1; i < a_data->verticies_count; i++)
                {
                    float angle = step * i + a_data->angle;
                    vec vert_pos = { cos(angle) * a_data->verticies[i], sin(angle) * a_data->verticies[i] };
                    vert_pos = vec_sum(a_data->pos, vert_pos);
                    vert_pos = vec_sub(vert_pos, camera_pos);
                    drawLines(vert_temp_pos.x, vert_temp_pos.y, vert_pos.x, vert_pos.y, color);
                    vert_temp_pos = vert_pos;
                }
                drawLines(vert_temp_pos.x, vert_temp_pos.y, first_pos.x, first_pos.y, color);
            }
            for (list_elem* i = map.cells[cell_x][cell_y].bullets.begin; i != NULL; i = i->next)
            {
                Bullet* b = i->data;
                setPixels(b->pos.x - camera_pos.x, b->pos.y - camera_pos.y, 0x34);
            }
        }
    }
}

static uint8 bullet_check_collisions(int32 cell_x, int32 cell_y, Bullet* b)
{
    uint8 collided = 0;
    for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL;)
    {
        Asteroid* a = (Asteroid*)i->data;
        if (vec_distance(b->pos, a->pos) <= a->size)
        {
            a->damaged = 1;
            a->health -= bullet_damage;
            if (a->health <= 0)
            {
                list_elem* i_next = i->next;
                list_remove_by_elem(&map.cells[cell_x][cell_y].asteroids, i);
                asteroid_delete(a); // TODO
                score++;
                if (score % 80 == 0)
                    player_health = min(3, player_health + 1);
                i = i_next;
            }
            else i = i->next;
            collided = 1;
        }
        else i = i->next;
    }
    return collided;
}

static void iterate_cells_logic()
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
                a_data->angle += a_data->rotation_speed;

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
                    map_push_asteroid(&map, a_data);
                    i = i_next;
                }
                else i = i->next;
            }
            for (list_elem* i = map.cells[cell_x][cell_y].bullets.begin; i != NULL;)
            {
                Bullet* b = i->data;
                b->pos = vec_sum(b->pos, b->velocity);
                b->velocity = vec_sum(b->velocity, b->acceleration);

                uint8 collided = 0;

                collided = max(bullet_check_collisions(cell_x, cell_y, b), collided);
                uint8 left = 0, right = 0, top = 0, bottom = 0;
                float a_size = asteroid_max_size * ASTEROID_SIZE_MAX;
                if (b->pos.x - map.corner00_pos.x - map.cell_size * cell_x <= a_size)
                {
                    left = 1;
                    collided = max(bullet_check_collisions(cell_x - 1, cell_y, b), collided);
                }
                if (b->pos.y - map.corner00_pos.y - map.cell_size * cell_y <= a_size)
                {
                    top = 1;
                    collided = max(bullet_check_collisions(cell_x, cell_y - 1, b), collided);
                }
                if (map.corner00_pos.x + map.cell_size * (cell_x + 1) - b->pos.x <= a_size)
                {
                    right = 1;
                    collided = max(bullet_check_collisions(cell_x + 1, cell_y, b), collided);
                }
                if (map.corner00_pos.y + map.cell_size * (cell_y + 1) - b->pos.y <= a_size)
                {
                    bottom = 1;
                    collided = max(bullet_check_collisions(cell_x, cell_y + 1, b), collided);
                }
                if (left && top)
                    collided = max(bullet_check_collisions(cell_x - 1, cell_y - 1, b), collided);
                if (left && bottom)
                    collided = max(bullet_check_collisions(cell_x - 1, cell_y + 1, b), collided);
                if (right && top)
                    collided = max(bullet_check_collisions(cell_x + 1, cell_y - 1, b), collided);
                if (right && bottom)
                    collided = max(bullet_check_collisions(cell_x + 1, cell_y + 1, b), collided);

                if (collided)
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].bullets, i);
                    bullet_delete(b);
                    i = i_next;
                    continue;
                }

                int32 c_x = b->pos.x - map.corner00_pos.x;
                c_x /= map.cell_size;
                int32 c_y = b->pos.y - map.corner00_pos.y;
                c_y /= map.cell_size;
                if (c_x != cell_x || c_y != cell_y)
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].bullets, i);
                    map_push_bullet(&map, b);
                    i = i_next;
                }
                else i = i->next;
            }
        }
    }
} 

static void display_ui()
{
    putString(1, 1, ui_color, "Score: %d", score);
    if (player_health > 0)
    {
        int16 start_x = screen_width - (heart_size_x + 1) * player_health;
        for (int i = 0; i < player_health; i++)
        {
            put_heart(start_x, 1, ui_color);
            start_x += heart_size_x + 1;
        }
    }
}

void game_init() {
    dbg_printf("Game init\n");
    init_values();
    player_constructor((vec){ 0, 0 }, (vec){ 0, 0 }, (vec) { 0, 0 }, 0, 0.023); 
    map_constructor(&map, (ivec){ 0, 0 }, 60, 30);
    init_stars();

    add_event(spawn_asteroids, asteroid_spawn_time);
    add_event(increase_difficulty1, increase_difficulty_time1);
    camera_pos.x = -(int32)screen_width / 2;
    camera_pos.y = -(int32)screen_height / 2;
    spawn_start_asteroids();
}

void game_input() {
    player_input();
    if (key_press(SC_ESCAPE)) run_current_scene = 0;
}

void game_logic() {
    player_logic();
    iterate_cells_logic();
    stars_logic();
}

void game_graphics() {
    stars_graphics();
    iterate_cells_graphics();
    player_graphics();
    display_ui();
}

struct Scene game_destruct() {
    map_destructor(&map);
    destruct_stars();
    clear_events();

    return (struct Scene) {
        gameover_init,
        gameover_input,
        gameover_logic,
        gameover_graphics,
        gameover_destruct,
    };
}