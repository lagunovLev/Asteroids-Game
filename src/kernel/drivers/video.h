#pragma once
#include "../common.h"

extern const uint8* video_memory;
extern const uint32 screen_width;
extern const uint32 screen_height;
extern const uint8* double_buffer;

void setPixel(uint16 x, uint16 y, uint8 color);
void drawFillRect(uint16 x, uint16 y, uint16 width, uint16 height, uint8 color);
void drawFillCircle(uint16 x, uint16 y, uint16 r, uint8 color);
void clearScreen(uint8 color);
void putChar(uint16 x, uint16 y, char ch, uint8 color);
void putString(uint16 x, uint16 y, uint8 color, char* string, ...);
void flip();