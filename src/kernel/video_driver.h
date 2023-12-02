#include "header.h"

uint8* video_memory = 0xA0000;
uint32 screen_width = 320;
uint32 screen_height = 200;

void setPixel(uint32 x, uint32 y, uint8 color)
{
    uint32 offset = x + y * screen_width;
    *(video_memory + offset) = color;
}