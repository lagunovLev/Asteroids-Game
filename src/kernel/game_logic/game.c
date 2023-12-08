#include "game.h"



void game_init() {

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

}
