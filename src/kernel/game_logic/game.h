#pragma once 
#include "../common.h"
#include "../debug.h"
#include "scene.h" 

extern int32 score;

void game_init();
void game_input();
void game_logic();
void game_graphics();
struct Scene game_destruct();