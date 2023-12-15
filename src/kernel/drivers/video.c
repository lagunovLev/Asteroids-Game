#include "video.h"
#include "../math.h"
#include "font.h"
#include <stdarg.h>
#include "../debug.h"

void vsync_wait();

const uint8* video_memory = (const uint8*)0xA0000;
const uint32 screen_width = 320;
const uint32 screen_height = 200;
const uint8* double_buffer = (const uint8*)0x100000; 

void drawFillRect(int16 x, int16 y, uint16 width, uint16 height, uint8 color)
{
    for (uint16 i = y; i < y + height; i++)
    {
        int32 offset = i * screen_width;
        for (uint16 j = x; j < x + width; j++)
        {
            *((uint8*)double_buffer + offset + j) = color;
        }
    }
}

void drawFillCircle(int16 x, int16 y, uint16 r, uint8 color)
{
    for (int i = -r; i <= r; i++)
    {
        for (int j = -r; j <= r; j++)
        {
            if (r > i * i + j * j)
                setPixel(x + i, y + j, color);
        }
    }
}

void drawFillRects(int16 x, int16 y, uint16 width, uint16 height, uint8 color) {
    for (uint16 i = y; i < y + height; i++)
    {
        for (uint16 j = x; j < x + width; j++)
        {
            setPixels(j, i, color);
        }
    }
}

void drawFillCircles(int16 x, int16 y, uint16 r, uint8 color) {
    for (int i = -r; i <= r; i++)
    {
        for (int j = -r; j <= r; j++)
        {
            if (r > i * i + j * j)
                setPixels(x + i, y + j, color);
        }
    }
}

void drawLines(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color)
{
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    setPixels(x2, y2, color);
    while(x1 != x2 || y1 != y2) 
    {
        setPixels(x1, y1, color);
        int error2 = error * 2;
        if(error2 > -deltaY) 
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX) 
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

void drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color)
{
    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    setPixel(x2, y2, color);
    while(x1 != x2 || y1 != y2) 
    {
        setPixel(x1, y1, color);
        int error2 = error * 2;
        if(error2 > -deltaY) 
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX) 
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

void clearScreen(uint8 color)
{
    for (uint8* i = (uint8*)double_buffer; i != double_buffer + screen_height * screen_width; i++)
        *(i) = color;
}

static inline void drawCharLayer(int32 offset, char layer, uint8 color)
{
    offset += char_width;
    for (uint8 i = 0; i < char_width; i++)
    {
        if (layer % 2)
            *((uint8*)double_buffer + offset) = color;
        layer >>= 1;
        offset--;
    }
}

static void printChar(int32 offset, char ch, uint8 color)
{
    uint16 char_start = ch * char_height;
    for (uint8 i = 0; i < char_height; i++)
    {
        drawCharLayer(offset, ((char*)font)[char_start], color);
        char_start++;
        offset += screen_width;
    }
} 

void putChar(int16 x, int16 y, char ch, uint8 color)
{
    int32 offset = x + y * screen_width;

    if (ch == '\n' || ch == '\t' || ch == '\r' || ch == '\0')
        return;
    printChar(offset, ch, color);
}

static void printString(int32* str_offset, uint8 color, char* string)
{
    for (uint16 i = 0; string[i]; i++)
    {
        if (string[i] == '\n')
        {
            *str_offset += (char_height + 1) * screen_width;
        } 
        else if (string[i] == '\t')
        {
            *str_offset += char_width * 4;
        }
        else if (string[i] == '\r')
        {
            *str_offset -= *str_offset % screen_width;
        }
        else 
        {
            printChar(*str_offset, string[i], color);
            *str_offset += char_width;
        }
    }
}

void putString(int16 x, int16 y, uint8 color, char* string, ...)
{
    int32 str_offset = x + y * screen_width;
    va_list arg_ptr;
    va_start(arg_ptr, string);
    char buf[256] = { 0 };
    vsprintf(buf, string, arg_ptr);
    printString(&str_offset, color, buf);
    va_end(arg_ptr);
}

void flip()
{
    vsync_wait();
    memcpy(video_memory, double_buffer, screen_height * screen_width);
}