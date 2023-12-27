#include "gameover.h"
#include "game.h"
#include "../kernel.h"
#include "../drivers/keyboard.h"
#include "../drivers/font.h"
#include "../drivers/video.h"

int32 pos_x1;
int32 pos_y1;
int32 pos_x2;
int32 pos_y2;
char text1[32];
char* text2 = "Press enter to retry";

void gameover_init()
{
    sprintf(text1, "Your score is: %d", score);
    pos_x1 = (screen_width - strlen(text1) * char_width) / 2;
    pos_y1 = (screen_height - char_height) / 2;
    pos_y2 = pos_y1 + char_height;
    pos_x2 = (screen_width - strlen(text2) * char_width) / 2;
}

void gameover_input()
{
    if (key_press(SC_ENTER))
        run_current_scene = 0;
    if (key_press(SC_ESCAPE))
    {
        run_current_scene = 0;
        run = 0;
    }
}

void gameover_logic()
{

}

void gameover_graphics()
{
    clearScreen(0x00);
    putString(pos_x1, pos_y1, 0x0F, text1);
    putString(pos_x2, pos_y2, 0x0F, text2);
}

struct Scene gameover_destruct()
{
    return (struct Scene) {
        game_init,
        game_input,
        game_logic,
        game_graphics,
        game_destruct,
    };
}

