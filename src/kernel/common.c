#include "common.h"
#include "drivers/video.h"

uint16 strlen(char* str)
{
    uint16 i = 0;
    while (str[i])
        i++;
    return i;
}

uint32 abs(int32 n)
{
    if (n < 0) return -n;
    return n;
}

static uint16 number_of_digits(int32 n)
{
    uint16 i = 1;
    while (abs(n) >= 10)
    {
        n /= 10;
        i++;
    }
    return i;
}

void int_to_string(int32 n, char* str)
{
    uint16 i = 0;
    if (n < 0)
    {
        str[i] = '-';
        i++;
    }
    n = abs(n);

    uint16 num_of_digits = number_of_digits(n);
    uint32 t = n;
    int32 g = 0;
    for (int16 pos = i + num_of_digits - 1; pos != i - 1; pos--)
    {
        str[pos] = t % 10 + 48;
        g = t % 10;
        t /= 10;
    }
    str[i] = g + 48;
    str[i + num_of_digits] = '\0';
}

void memcpy(uint8* dest, uint8* src, uint32 size)
{
    for (uint32 i = 0; i < size; i++)
    {
        *(dest + i) = *(src + i);
    }
}

uint8* current_memory;

void malloc_init()
{
    current_memory = (uint8*)double_buffer + screen_height * screen_width;
}

void* malloc(uint32 n)
{
    uint8* res = current_memory;
    current_memory += n;
    return res;
}

void memset(uint8* dest, uint8 byte, uint32 size)
{
    for (uint8* i = dest; i < dest + size; i++)
        *i = byte;
}

static unsigned long int next = 0;

uint32 rand()
{
    next = next * 1103515245 + 12345;
    return (unsigned int) (next / 65536) % 32768;
}

void srand(uint32 seed)
{
    next = seed;
}

float rand_float(float f1, float f2)
{
    return f1 + ((float)rand() / RAND_MAX * (f2 - f1));
}

uint32 rand_int(uint32 a, uint32 b)
{
    return a + rand() % (b - a + 1); 
}
