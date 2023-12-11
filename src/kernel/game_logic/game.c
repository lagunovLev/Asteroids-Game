#include "game.h"
#include "../util/list.h"
#include "asteroid.h"
#include "../drivers/video.h"
#include "../kernel.h"
#include "../debug.h"
#include "../drivers/keyboard.h"

char* str = "";
List test_list;

void game_init() {
    list_constructor(&test_list);
    list_push_back(&test_list, asteroid_new(5, 7));
    list_push_back(&test_list, asteroid_new(1488, 69));
    list_push_back(&test_list, asteroid_new(89, 45));
    list_push_back(&test_list, asteroid_new(1939, 1945));
    list_insert(&test_list, asteroid_new(67, 12), -1);
    asteroid_delete(list_remove(&test_list, -5));
}

void game_input() {
    if (key_none(72)) 
        str = "None";
    if (key_press(72)) 
        str = "Press";
    if (key_release(72)) 
        run = 0;
    if (key_repeat(72)) 
        str = "Repeat";
}

void game_logic() {

}

void game_graphics() {
    putString(0, 15 * 8, 0x65, "%s", str);
    
    uint32 j = 0; 
    for (list_elem* i = test_list.begin; i != NULL; i = i->next)
    {
        Asteroid* a_data = (Asteroid*)i->data;
        putString(0, j * 9, 0x65, "x: %d, y: %d", a_data->x, a_data->y);
        j++;
    }
}

void game_destruct() {
    list_destructor(&test_list, asteroid_delete);
}