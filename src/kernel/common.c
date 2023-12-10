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

uint8* header_start;
uint8* heap_start;
uint32 heap_size;
uint32 header_size;
uint32 pages;
uint32 page_size;
uint32 cur_page;
uint8 reached_limit;

static uint8 check_free_pages(uint32 pages_occupies, uint32* next_page)
{
    if (cur_page + pages_occupies >= pages - 1)
    {
        if (reached_limit)
            asm volatile("int $20");
        reached_limit = 1;
        cur_page = 0;
    }
    for (uint32 cur_checking_page = cur_page; cur_checking_page < pages_occupies + cur_page; cur_checking_page++)
    {
        uint8* p_ptr = header_start + cur_checking_page;
        if (*p_ptr)
        {
            *next_page = cur_checking_page + 1;
            return 0;
        }
    }
    return 1;
}

static uint32 find_free_pages(uint32 pages_occupies)
{ 
    reached_limit = 0;
    while (1)
    {
        uint32 next_page = 0;
        if (check_free_pages(pages_occupies, &next_page))
        {
            cur_page += pages_occupies;
            return cur_page - pages_occupies;
        }
        cur_page = next_page;
    }
}

void malloc_init()
{
    header_start = (uint8*)double_buffer + 65536;
    pages = 1000000;
    page_size = 1024;
    header_size = 1 * pages;
    heap_start = header_start + header_size;
    heap_size = pages * page_size;
    cur_page = 0;
    memset(header_start, 0, header_size);
    reached_limit = 0;
}

void* malloc(uint32 n)
{
    // isOccupied - 1 byte, how many pages - 4 bytes
    n += 5;
    uint32 pages_occupies = n / page_size - (n % page_size == 0 ? 1 : 0) + 1;
    uint32 start_page = find_free_pages(pages_occupies);
    memset(header_start + start_page, 1, pages_occupies);
    uint8* addr = heap_start + start_page * page_size;
    *addr = 1;
    *((uint32*)(addr + 1)) = pages_occupies;
    return addr + 5;
}

void free(void* ptr)
{
    uint8* iptr = (uint8*)ptr - 5;
    uint8 is_occupied = *iptr;
    if (!is_occupied) {
        asm volatile("int $19");
    }
    uint32 pages_occupied = *((uint32*)(iptr + 1));
    memset(iptr, 0, 5);
    uint32 page_number = ((uint32)(iptr) - (uint32)(heap_start)) / page_size;
    memset((header_start + page_number), 0, pages_occupied);
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
