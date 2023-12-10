#include "game.h"
#include "../util/list.h"
#include "asteroid.h"
#include "../drivers/video.h"

char* str = "";
//List test_list;

void print_list(void* data, uint32 index)
{
    Asteroid* a_data = (Asteroid*)data;
    putString(0, index * 9, 0x65, "x: %d, y: %d", a_data->x, a_data->y);
}

void game_init() {
    //list_constructor(&test_list);
    //list_push_back(&test_list, asteroid_new(5, 7));
    //list_push_back(&test_list, asteroid_new(1488, 69));
    //list_push_front(&test_list, asteroid_new(89, 45));
    //list_insert(&test_list, asteroid_new(67, 12), -2);
    //asteroid_delete(list_remove(&test_list, 2));
}

void game_input() {
    if (key_none(72)) 
        str = "None";
    if (key_press(72)) 
        str = "Press";
    if (key_release(72)) 
        str = "Release";
    if (key_repeat(72)) 
        str = "Repeat";
}

void game_logic() {

}

void game_graphics() {
    putString(0, 15 * 8, 0x65, "%s", str);
    //list_iter(&test_list, print_list);
}
