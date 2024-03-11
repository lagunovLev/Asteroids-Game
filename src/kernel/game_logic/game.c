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
int32 shoot_cooldown;
uint8 reloading;
float strong_asteroid_chance;
float bullet_speed;
float bullet_damage;
float asteroid_min_volume;
float player_mass;
float asteroid_mass_mul;
float collide_strength;

int32 increase_difficulty_time1;
int32 increase_difficulty_time2;
int32 increase_difficulty_time3;
int32 increase_difficulty_time4;
int32 increase_difficulty_time5;

static void init_values()
{
    asteroid_spawn_time = 140; 
    border_pushing = 0.1f;
    border_width_cells = 5;
    asteroid_min_size = 7;
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
    bullet_speed = 3;
    bullet_damage = 100;
    asteroid_min_volume = asteroid_min_size * asteroid_min_size * PI;
    player_mass = 200;
    asteroid_mass_mul = 2;
    collide_strength = 0.5;

    increase_difficulty_time1 = 2000;
    increase_difficulty_time2 = 3500;
    increase_difficulty_time3 = 5000;
    increase_difficulty_time4 = 6500;
    increase_difficulty_time5 = 8000;
}

static void increase_difficulty5()
{
    asteroid_spawn_time = 70;
    strong_asteroid_chance = 1;
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty4()
{
    asteroid_spawn_time = 90;
    strong_asteroid_chance = 1;
    add_event(increase_difficulty5, increase_difficulty_time5);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty3()
{
    asteroid_spawn_time = 95;
    strong_asteroid_chance = 0.7;
    add_event(increase_difficulty4, increase_difficulty_time4);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty2()
{
    asteroid_spawn_time = 105;
    strong_asteroid_chance = 0.4;
    add_event(increase_difficulty3, increase_difficulty_time3);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty1()
{
    asteroid_spawn_time = 120;
    add_event(increase_difficulty2, increase_difficulty_time2);
    dbg_printf("Increased difficulty!\n");
}

static void increase_difficulty0()
{
    add_event(increase_difficulty1, increase_difficulty_time1);
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

    float speed = rand_float(0.08, 0.8);
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
    float a = player.rotation_angle + rand_float(-0.05, 0.05);
    float p_x = player.position_current.x + cos(player.rotation_angle) * player_size;
    float p_y = player.position_current.y + sin(player.rotation_angle) * player_size;
    float v_x = cos(a) * bullet_speed;
    float v_y = sin(a) * bullet_speed;
    map_push_bullet(&map, bullet_new((vec){ p_x, p_y }, (vec){ v_x, v_y }, (vec){ 0, 0 }));
    add_event(reloading_end, shoot_cooldown);
}

static void player_input()
{
    if (!key_none(SC_W))
    {
        player.acceleration.x += cos(player.rotation_angle) * player.speed;
        player.acceleration.y += sin(player.rotation_angle) * player.speed;
    }
    if (!key_none(SC_S))
    {
        player.acceleration.x -= cos(player.rotation_angle) * player.speed;
        player.acceleration.y -= sin(player.rotation_angle) * player.speed;
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
    if (key_press(SC_ENTER))
        dbg_printf("Enter pressed!\n");
}

static void check_player_collisions(int32 cell_x, int32 cell_y)
{
    //dbg_printf("func check_player_collisions start\n");
    for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL; i = i->next)
    {
        dbg_puts("pc");
        Asteroid* a = i->data;
        float distance = vec_distance(a->position_current, player.position_current);
        if (distance < player_size / 2 + a->size)
        {
            if (is_player_vulnerable)
            {
                player_health--;
                if (player_health <= 0)
                    run_current_scene = 0;
                is_player_vulnerable = 0;
                player_vulnerable_event1();
                add_event(player_invulnerable_end, player_invulnerable_duration);
            }

            vec axis = vec_sub(player.position_current, a->position_current);
            vec n = vec_norm(axis);
            float delta = player_size / 2 + a->size - distance;

            float whole_mass = player_mass + asteroid_mass(a) * asteroid_mass_mul;
            float mul1 = player_mass / whole_mass * collide_strength;
            float mul2 = asteroid_mass(a) * asteroid_mass_mul / whole_mass * collide_strength;
            player.position_current = vec_sum(player.position_current, vec_mul(n, mul2 * delta));
            a->position_current = vec_sub(a->position_current, vec_mul(n, mul1 * delta));
        }
    }
    //dbg_printf("func check_player_collisions end\n");
}

static void collide_asteroids(Asteroid* a1, Asteroid* a2, float distance)
{
    //dbg_printf("func collide_asteroids start\n");
    vec axis = vec_sub(a1->position_current, a2->position_current);
    vec n = vec_norm(axis);
    float delta = a1->size + a2->size - distance;

    float whole_mass = (asteroid_mass(a2) + asteroid_mass(a1)) * asteroid_mass_mul;
    float mul1 = asteroid_mass(a2) * asteroid_mass_mul / whole_mass * collide_strength;
    float mul2 = asteroid_mass(a1) * asteroid_mass_mul / whole_mass * collide_strength;
    a1->position_current = vec_sum(a1->position_current, vec_mul(n, mul1 * delta));
    a2->position_current = vec_sub(a2->position_current, vec_mul(n, mul2 * delta));
    //dbg_printf("func collide_asteroids end\n");
}

static void collide_asteroid(Asteroid* a_main, int32 cell_x, int32 cell_y)
{
    //dbg_printf("func collide_asteroid start\n");
    for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL; i = i->next)
    {
        dbg_puts("ac");
        Asteroid* a = i->data;
        if (a == a_main)
            continue;
        float distance = vec_distance(a->position_current, a_main->position_current);
        if (distance < a_main->size + a->size)
            collide_asteroids(a_main, a, distance);
    }
    //dbg_printf("func collide_asteroid end\n");
}

static void player_logic()
{
    //dbg_printf("func player_logic start\n");
    if (player.position_current.x < map.corner00_pos.x + border_width_cells * map.cell_size)
        player.acceleration.x += border_pushing;
    if (player.position_current.y < map.corner00_pos.y + border_width_cells * map.cell_size)
        player.acceleration.y += border_pushing;
    if (player.position_current.x > map.corner11_pos.x - border_width_cells * map.cell_size)
        player.acceleration.x -= border_pushing;
    if (player.position_current.y > map.corner11_pos.y - border_width_cells * map.cell_size)
        player.acceleration.y -= border_pushing;

    int32 cell_player_x = (player.position_current.x - map.corner00_pos.x) / map.cell_size;
    int32 cell_player_y = (player.position_current.y - map.corner00_pos.y) / map.cell_size;

    check_player_collisions(cell_player_x, cell_player_y);
    uint8 left = 0, right = 0, top = 0, bottom = 0;
    float a_size = asteroid_max_size * ASTEROID_SIZE_MAX + player_size / 2;
    if (player.position_current.x - map.corner00_pos.x - map.cell_size * cell_player_x <= a_size)
    {
        left = 1;
        check_player_collisions(cell_player_x - 1, cell_player_y);
    }
    if (player.position_current.y - map.corner00_pos.y - map.cell_size * cell_player_y <= a_size)
    {
        top = 1;
        check_player_collisions(cell_player_x, cell_player_y - 1);
    }
    if (map.corner00_pos.x + map.cell_size * (cell_player_x + 1) - player.position_current.x <= a_size)
    {
        right = 1;
        check_player_collisions(cell_player_x + 1, cell_player_y);
    }
    if (map.corner00_pos.y + map.cell_size * (cell_player_y + 1) - player.position_current.y <= a_size)
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

    vec velocity = player_velocity(&player);
    velocity = vec_mul(velocity, 0.99);
    player.position_old = player.position_current;
    player.position_current = vec_sum(player.position_current, vec_sum(velocity, player.acceleration));
    player.acceleration = (vec){ 0, 0 };

    camera_pos = vec_sub(player.position_current, (vec){ screen_width / 2, screen_height / 2 });
    camera_pos.x = dmax(camera_pos.x, map.corner00_pos.x + border_width_cells * map.cell_size);
    camera_pos.y = dmax(camera_pos.y, map.corner00_pos.y + border_width_cells * map.cell_size);
    camera_pos.x = dmin(camera_pos.x, map.corner11_pos.x - border_width_cells * map.cell_size - (int32)screen_width);
    camera_pos.y = dmin(camera_pos.y, map.corner11_pos.y - border_width_cells * map.cell_size - (int32)screen_height);
    //dbg_printf("func player_logic end\n");
}

static void player_graphics()
{
    dbg_puts("pg");
    vec player_pos = vec_sub(player.position_current, camera_pos);
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
                dbg_puts("ga");
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

                if (!isInCamera(a_data->position_current, a_data->size * ASTEROID_SIZE_MAX))
                    continue;

                dbg_puts("ga2");
                float step = 2 * PI / a_data->verticies_count;
                vec vert_temp_pos = { cos(a_data->angle) * a_data->verticies[0], sin(a_data->angle) * a_data->verticies[0] };
                vert_temp_pos = vec_sum(a_data->position_current, vert_temp_pos);
                vert_temp_pos = vec_sub(vert_temp_pos, camera_pos);
                vec first_pos = vert_temp_pos;
                for (int i = 1; i < a_data->verticies_count; i++)
                {
                    float angle = step * i + a_data->angle;
                    vec vert_pos = { cos(angle) * a_data->verticies[i], sin(angle) * a_data->verticies[i] };
                    vert_pos = vec_sum(a_data->position_current, vert_pos);
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

static void destroy_asteroid(Asteroid* a)
{
    //dbg_printf("func destroy_asteroid start\n");
    float volume = a->size * a->size * PI;
    volume *= 0.9;
    float mass = volume;
    if (a->strong)
        mass *= 2;

    if (volume > asteroid_min_volume * 2)
    {
        float volume1 = rand_float(asteroid_min_volume, volume - asteroid_min_volume);
        float volume2 = volume - volume1;
        float size1 = sqrt(volume1 / PI);
        float size2 = sqrt(volume2 / PI);
        float mass1 = volume1;
        float mass2 = volume2;
        if (a->strong)
        {
            mass1 *= 2;
            mass2 *= 2;
        }

        vec velocity = asteroid_velocity(a);

        float impulse_x = velocity.x * mass; 
        float impulse_y = velocity.y * mass;

        float angle = rand_float(0, 2 * PI);

        float impulse_x1 = cos(angle) * 100 + 0.5 * impulse_x;
        float impulse_y1 = sin(angle) * 100 + 0.5 * impulse_y;
        float impulse_x2 = cos(-angle) * 100 + 0.5 * impulse_x;
        float impulse_y2 = sin(-angle) * 100 + 0.5 * impulse_y;

        //if (impulse_x1 > 500)
        //{
        //    impulse_x1 -= 100;
        //    impulse_x2 += 100;
        //}
        //if (impulse_x2 > 500)
        //{
        //    impulse_x2 -= 100;
        //    impulse_x1 += 100;
        //}
        //if (impulse_y1 > 500)
        //{
        //    impulse_y1 -= 100;
        //    impulse_y2 += 100;
        //}
        //if (impulse_y2 > 500)
        //{
        //    impulse_y2 -= 100;
        //    impulse_y1 += 100;
        //}

        float velocity_x1 = impulse_x1 / mass1;
        float velocity_x2 = impulse_x2 / mass2;
        float velocity_y1 = impulse_y1 / mass1;
        float velocity_y2 = impulse_y2 / mass2;

        float delta = size1 + size2 / 2;
        delta *= 1.2;
        float delta_x_1 = cos(angle) * delta;
        float delta_y_1 = sin(angle) * delta;
        float delta_x_2 = cos(-angle) * delta;
        float delta_y_2 = sin(-angle) * delta;

        vec pos1 = vec_sum(a->position_current, (vec){ delta_x_1, delta_y_1 });
        vec pos2 = vec_sum(a->position_current, (vec){ delta_x_2, delta_y_2 });

        map_push_asteroid(&map, asteroid_new(pos1, (vec){ velocity_x1, velocity_y1 }, (vec){ 0, 0 }, size1, a->strong));
        map_push_asteroid(&map, asteroid_new(pos2, (vec){ velocity_x2, velocity_y2 }, (vec){ 0, 0 }, size2, a->strong));
    }
    //dbg_printf("func destroy_asteroid end\n");
}

static uint8 bullet_check_collisions(int32 cell_x, int32 cell_y, Bullet* b)
{
    uint8 collided = 0;
    for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL;)
    {
        dbg_puts("bc");
        Asteroid* a = (Asteroid*)i->data;
        if (vec_distance(b->pos, a->position_current) <= a->size)
        {
            a->damaged = 1;
            a->health -= bullet_damage;
            if (a->health <= 0)
            {
                list_elem* i_next = i->next;
                list_remove_by_elem(&map.cells[cell_x][cell_y].asteroids, i);
                destroy_asteroid(a);
                asteroid_delete(a); 
                score++;
                if (score % 100 == 0)
                {
                    player_health = min(3, player_health + 1);
                    if (score % 300 == 0)
                    {
                        shoot_cooldown = max(0, shoot_cooldown - 1);
                    }
                }
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
    //dbg_printf("func iterate_cells_logic start\n");
    for (int cell_x = 0; cell_x < map.side_size; cell_x++)
    {
        for (int cell_y = 0; cell_y < map.side_size; cell_y++)
        {
            for (list_elem* i = map.cells[cell_x][cell_y].asteroids.begin; i != NULL;)
            {
                dbg_putc('a');
                Asteroid* a_data = (Asteroid*)i->data;
                if (a_data->moved)
                {
                    a_data->moved = 0;
                    continue;
                }
                a_data->angle += a_data->rotation_speed;

                if (a_data->position_current.x < map.corner00_pos.x + border_width_cells * map.cell_size)
                    a_data->acceleration.x += border_pushing;
                if (a_data->position_current.y < map.corner00_pos.y + border_width_cells * map.cell_size)
                    a_data->acceleration.y += border_pushing;
                if (a_data->position_current.x > map.corner11_pos.x - border_width_cells * map.cell_size)
                    a_data->acceleration.x -= border_pushing;
                if (a_data->position_current.y > map.corner11_pos.y - border_width_cells * map.cell_size)
                    a_data->acceleration.y -= border_pushing;
                
                vec velocity = asteroid_velocity(a_data);
                if (vec_length(velocity) > 3)
                {
                    vec g = vec_norm(velocity);
                    velocity = vec_mul(g, 3);
                }
                a_data->position_old = a_data->position_current;
                a_data->position_current = vec_sum(a_data->position_current, vec_sum(velocity, a_data->acceleration)); 
                a_data->acceleration = (vec){ 0, 0 };

                int32 c_x = a_data->position_current.x - map.corner00_pos.x;
                c_x /= map.cell_size;
                int32 c_y = a_data->position_current.y - map.corner00_pos.y;
                c_y /= map.cell_size;
                ///////////////////////////////////////////////////////////
                collide_asteroid(a_data, c_x, c_y);
                uint8 left = 0, right = 0, top = 0, bottom = 0;
                float a_size = asteroid_max_size * ASTEROID_SIZE_MAX * 2;
                if (a_data->position_current.x - map.corner00_pos.x - map.cell_size * c_x <= a_size)
                {
                    left = 1;
                    collide_asteroid(a_data, c_x - 1, c_y);
                }
                if (a_data->position_current.y - map.corner00_pos.y - map.cell_size * c_y <= a_size)
                {
                    top = 1;
                    collide_asteroid(a_data, c_x, c_y - 1);
                }
                if (map.corner00_pos.x + map.cell_size * (c_x + 1) - a_data->position_current.x <= a_size)
                {
                    right = 1;
                    collide_asteroid(a_data, c_x + 1, c_y);
                }
                if (map.corner00_pos.y + map.cell_size * (c_y + 1) - a_data->position_current.y <= a_size)
                {
                    bottom = 1;
                    collide_asteroid(a_data, c_x, c_y + 1);
                }
                if (left && top)
                    collide_asteroid(a_data, c_x - 1, c_y - 1);
                if (left && bottom)
                    collide_asteroid(a_data, c_x - 1, c_y + 1);
                if (right && top)
                    collide_asteroid(a_data, c_x + 1, c_y - 1);
                if (right && bottom)
                    collide_asteroid(a_data, c_x + 1, c_y + 1);
                ////////////////////////////////////////////////////////////////////////////////////////
                if (c_x != cell_x || c_y != cell_y)
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].asteroids, i);
                    map_push_asteroid(&map, a_data);
                    a_data->moved = 1;
                    i = i_next;
                }
                else i = i->next;
            }
            for (list_elem* i = map.cells[cell_x][cell_y].bullets.begin; i != NULL;)
            {
                dbg_putc('b');
                Bullet* b = i->data;
                if (b->moved)
                {
                    b->moved = 0;
                    continue;
                }
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
                if (!((c_x > border_width_cells - 1 && c_x < map.side_size - border_width_cells) && 
                      (c_y > border_width_cells - 1 && c_y < map.side_size - border_width_cells)))
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].bullets, i);
                    i = i_next;
                    continue;
                }
                if (c_x != cell_x || c_y != cell_y)
                {
                    list_elem* i_next = i->next;
                    list_remove_by_elem(&map.cells[cell_x][cell_y].bullets, i);
                    uint8 pushed = map_push_bullet(&map, b);
                    b->moved = 1;
                    i = i_next;
                }
                else i = i->next;
            }
        }
    }
    //dbg_printf("func iterate_cells_logic end\n");
} 

static void display_ui()
{
    dbg_putc('d');
    putString(1, 1, ui_color, "Score: %d", score);
    dbg_putc('h');
    if (player_health > 0)
    {
        int16 start_x = screen_width - (heart_size_x + 1) * player_health;
        for (int i = 0; i < player_health; i++)
        {
            dbg_puts("h2");
            put_heart(start_x, 1, ui_color);
            dbg_puts("h3");
            start_x += heart_size_x + 1;
        }
    }
}

void game_init() {
    dbg_printf("Game init\n");
    init_values();
    player_constructor((vec){ 0, 0 }, (vec){ 0, 0 }, (vec) { 0, 0 }, 0, 0.027); 
    map_constructor(&map, (ivec){ 0, 0 }, 60, 35);
    init_stars();

    add_event(spawn_asteroids, asteroid_spawn_time);
    increase_difficulty0();
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
    dbg_puts("d2");
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