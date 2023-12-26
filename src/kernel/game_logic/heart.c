#include "heart.h"
#include "../drivers/video.h"

int heart_size_x = 9;
int heart_size_y = 9;
char heart[] = {
    0, 0, 1, 1, 0, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0,
};

void put_heart(int16 x, int16 y, uint8 color)
{
    for (int i = 0; i < heart_size_y; i++)
    {
        for (int j = 0; j < heart_size_x; j++)
        {
            if (heart[i * heart_size_x + j])
                setPixel(x + j, y + i, color);
        }
    }
}