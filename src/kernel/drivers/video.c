#include "video.h"
#include "../math.h"
#include "font.h"
#include <stdarg.h>

const uint8* video_memory = 0xA0000;
const uint32 screen_width = 320;
const uint32 screen_height = 200;
const uint8* double_buffer = 0x100000; // 0xAFA00

void setPixel(uint16 x, uint16 y, uint8 color)
{
    uint16 offset = x + y * screen_width;
    *((uint8*)double_buffer + offset) = color;
}

void drawFillRect(uint16 x, uint16 y, uint16 width, uint16 height, uint8 color)
{
    for (uint16 i = y; i < y + height; i++)
    {
        uint16 offset = i * screen_width;
        for (uint16 j = x; j < x + width; j++)
        {
            *((uint8*)double_buffer + offset + j) = color;
        }
    }
}

void drawFillCircle(uint16 x, uint16 y, uint16 r, uint8 color)
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

void clearScreen(uint8 color)
{
    for (uint8* i = double_buffer; i != double_buffer + screen_height * screen_width; i++)
        *(i) = color;
}

static inline void drawCharLayer(uint32 offset, char layer, uint8 color)
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

static void printChar(uint32 offset, char ch, uint8 color)
{
    uint16 char_start = ch * char_height;
    for (uint8 i = 0; i < char_height; i++)
    {
        drawCharLayer(offset, ((char*)font)[char_start], color);
        char_start++;
        offset += screen_width;
    }
} 

void putChar(uint16 x, uint16 y, char ch, uint8 color)
{
    uint32 offset = x + y * screen_width;

    if (ch == '\n' || ch == '\t' || ch == '\r' || ch == '\0')
        return;
    printChar(offset, ch, color);
}

static void printString(uint32* str_offset, uint8 color, char* string)
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

void putString(uint16 x, uint16 y, uint8 color, char* string, ...)
{
    uint8 percentage = 0;
    va_list arg_ptr;
    va_start(arg_ptr, string);
    uint32 str_offset = x + y * screen_width;
    for (uint16 i = 0; string[i]; i++)
    {
        if (string[i] == '\n')
        {
            str_offset += (char_height + 1) * screen_width;
        } 
        else if (string[i] == '\t')
        {
            str_offset += char_width * 4;
        }
        else if (string[i] == '\r')
        {
            str_offset -= str_offset % screen_width;
        }
        else if (string[i] == '%')
        {
            if (!percentage)
            {
                percentage = 1;
                continue;
            }
            else {
                printChar(str_offset, '%', color);
                str_offset += char_width;
            }
        }
        else if (percentage)
        {
            if (string[i] == 'd')
            {
                int32 d = va_arg(arg_ptr, int32);
                char buf[32];
                int_to_string(d, buf);
                printString(&str_offset, color, buf);
                //str_offset += strlen(buf) * char_width;
            }
            else if (string[i] == 's')
            {
                char* s = va_arg(arg_ptr, char*);
                printString(&str_offset, color, s);
                //str_offset += strlen(s) * char_width;
            }
            else 
            {
                printChar(str_offset, string[i], color);
                str_offset += char_width;
            }
        }
        else 
        {
            printChar(str_offset, string[i], color);
            str_offset += char_width;
        }
        percentage = 0;
    }
    va_end(arg_ptr);
}

void flip()
{
    memcpy(video_memory, double_buffer, screen_height * screen_width);
}