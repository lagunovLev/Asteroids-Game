#include "stars.h"
#include "../drivers/video.h"
#include "../debug.h"

uint8* starfield;
vec stars_pos;

void init_stars()
{
    starfield = malloc(screen_width * screen_height);
    for (uint32 i = 0; i < screen_width * screen_height; i++)
    {
        uint8 r = rand_int(0, 360);
        uint8 r2 = rand_int(0, 3);
        //dbg_printf("%d ", r2);
        uint8 color = 0;
        if (r2 == 0)
        {
            if (r <= 3)
                color = 0x0F;
            else if (r == 4)
                color = rand_int(0x40, 0x68);
            //else if (r == 5)
            //    color = rand_int(0x20, 0x3F);
        }
        starfield[i] = color;
    }
    stars_pos.x = 0;
    stars_pos.y = 0;
}

void destruct_stars()
{
    free(starfield);
}