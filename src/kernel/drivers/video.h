#pragma once
#include "../common.h"

extern const uint8* video_memory;
extern const uint32 screen_width;
extern const uint32 screen_height;
extern const uint8* double_buffer;

//void setPixel(int16 x, int16 y, uint8 color);
//void setPixels(int16 x, int16 y, uint8 color);
void drawFillRects(int16 x, int16 y, uint16 width, uint16 height, uint8 color);
void drawFillCircles(int16 x, int16 y, uint16 r, uint8 color);
void drawFillRect(int16 x, int16 y, uint16 width, uint16 height, uint8 color);
void drawFillCircle(int16 x, int16 y, uint16 r, uint8 color);
void drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color);
void drawLines(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color);
void clearScreen(uint8 color);
void putChar(int16 x, int16 y, char ch, uint8 color);
void putString(int16 x, int16 y, uint8 color, char* string, ...);
void flip();

static inline void setPixel(int16 x, int16 y, uint8 color)
{
    int32 offset = x + y * screen_width;
    *((uint8*)double_buffer + offset) = color;
}

static inline void setPixels(int16 x, int16 y, uint8 color) {
    if (x >= screen_width || y >= screen_height)
        return;
    int32 offset = x + y * screen_width;
    *((uint8*)double_buffer + offset) = color;
}