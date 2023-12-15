#include "stars.h"
#include "../drivers/video.h"

uint8* starfield;
vec stars_pos;

void init_stars()
{
    starfield = malloc(screen_width * screen_height);
    for (uint32 i = 0; i < screen_width * screen_height; i++)
    {
        uint8 r = rand_int(0, 100);
        starfield[i] = (r == 0 ? 0x0F : 0);
    }
    stars_pos.x = 0;
    stars_pos.y = 0;
}

void destruct_stars()
{
    free(starfield);
}