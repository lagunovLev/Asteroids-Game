#include "video_driver.h"

void kmain()
{
    for (uint8 i = 0; i < 256; i++)
        setPixel(i, 0, i);
}