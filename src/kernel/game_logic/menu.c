#include "menu.h"
#include "../drivers/video.h"
#include "../drivers/keyboard.h"
#include "game.h"
#include "../kernel.h"
#include "../drivers/font.h"

uint8 color;
int32 pos_x;
int32 pos_y;
char text[] = "Press enter to begin!";

void menu_init()
{
    color = 0x20;
    pos_x = (screen_width - sizeof(text) * char_width) / 2;
    pos_y = (screen_height - char_height) / 2;
}

void menu_input()
{
    if (!key_none(SC_ENTER))
        run_current_scene = 0;
}

void menu_logic()
{
    color++;
    if (color == 0x38)
        color = 0x20;
}

void menu_graphics()
{
    clearScreen(0x00);
    putString(pos_x, pos_y, color, text);
}

struct Scene menu_destruct()
{
    return (struct Scene) {
        game_init,
        game_input,
        game_logic,
        game_graphics,
        game_destruct,
    };
}

