#include "common.h"
#include "drivers/video.h"
#include <stdarg.h>
#include "debug.h"
#include "math.h"

uint16 strlen(char* str)
{
    uint16 i = 0;
    while (str[i])
        i++;
    return i;
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

static uint16 number_of_digits_hex(int32 n)
{
    uint16 i = 1;
    while (abs(n) >= 16)
    {
        n /= 16;
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

void hex_to_string(int32 n, char* str)
{
    uint16 i = 0;
    if (n < 0)
        str[i++] = '-';
    str[i++] = '0';
    str[i++] = 'x';
    n = abs(n);

    uint16 num_of_digits = number_of_digits_hex(n);
    uint32 t = n;
    int32 g = 0;
    for (int16 pos = i + num_of_digits - 1; pos != i - 1; pos--)
    {
        str[pos] = t % 16 + 48;
        if (str[pos] > 57)
            str[pos] += 7;
        g = t % 16;
        t /= 16;
    }
    str[i] = g + 48;
    if (str[i] > 57)
        str[i] += 7;
    str[i + num_of_digits] = '\0';
}

void float_to_string(double n1, char* str)
{
    uint16 i = 0;
    if (n1 < 0)
    {
        str[i++] = '-';
        n1 = -n1;
    }
    uint32 n = n1;

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

    if ((int)(n1) == n1)
    {
        str[i + num_of_digits] = '\0';
        return;
    }
    i += num_of_digits;
    str[i++] = '.';
    double f = n1 - n;
    uint16 a = 0;
    while (f != 0 && a < 6)
    {
        f *= 10;
        uint32 h = f;
        str[i++] = h + 48;
        f -= h;
        a++;
    }
    str[i] = '\0';
}

void memcpy(uint8* dest, uint8* src, uint32 size)
{
    for (uint32 i = 0; i < size; i++)
    {
        *(dest + i) = *(src + i);
    }
}

static uint8* header_start;
static uint8* heap_start;
static uint32 heap_size;
static uint32 header_size;
static uint32 pages;
static uint32 page_size;
static uint32 cur_page;
static uint8 reached_limit;
static uint32 malloc_calls = 0;
static uint32 free_calls = 0;

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
    page_size = 256;
    header_size = 1 * pages;
    heap_start = header_start + header_size;
    heap_size = pages * page_size;
    cur_page = 0;
    memset(header_start, 0, header_size);
    reached_limit = 0;
}

void malloc_destruct()
{
    if (malloc_calls > free_calls)
        dbg_printf("More malloc calls than free calls. Memory leak");
    else if (malloc_calls < free_calls)
        dbg_printf("More free calls than malloc calls. Memory freed multiplke times");
    else 
        dbg_printf("Memory is fine");
}

void* malloc(uint32 n)
{
    // isOccupied - 1 byte, how many pages - 4 bytes
    malloc_calls++;
    n += 5;
    uint32 pages_occupies = n / page_size - (n % page_size == 0 ? 1 : 0) + 1;
    uint32 start_page = find_free_pages(pages_occupies);
    memset(header_start + start_page, 1, pages_occupies);
    uint8* addr = heap_start + start_page * page_size;
    *addr = 1;
    *((uint32*)(addr + 1)) = pages_occupies;
    //dbg_printf("Allocating memory. Addres: %x. Pages number: %d. Start page: %d. Allocated bytes: %d\n", addr, pages_occupies, start_page, n - 5);
    //memset(addr + 5, n - 5, 0);
    return addr + 5;
}

void free(void* ptr)
{
    free_calls++;
    uint8* iptr = (uint8*)ptr - 5;
    uint8 is_occupied = *iptr;
    uint32 pages_occupied = *((uint32*)(iptr + 1));
    uint32 page_number = ((uint32)(iptr) - (uint32)(heap_start)) / page_size;
    //dbg_printf("Freeing memory. Addres: %x. Pages number: %d. Start page: %d.\n", iptr, pages_occupied, page_number);
    if (!is_occupied) {
        asm volatile("int $19");
    }
    memset(iptr, 0, 5);
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
    unsigned int next2 = next;
    int result;

    next2 *= 1103515245;
    next2 += 12345;
    result = (unsigned int) (next2 / 65536) % 2048;

    next2 *= 1103515245;
    next2 += 12345;
    result <<= 10;
    result ^= (unsigned int) (next2 / 65536) % 1024;

    next2 *= 1103515245;
    next2 += 12345;
    result <<= 10;
    result ^= (unsigned int) (next2 / 65536) % 1024;

    next = next2;

    return result;
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
    uint32 r = rand();
    uint32 res = a + r % (b - a + 1);
    //dbg_printf("%d %d %d %d\n", a, b, r, res);
    return res; 
}


void sprintf(char *s, const char *format, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(s, format, arg_ptr);
    va_end(arg_ptr);
}

void vsprintf(char *s, const char *format, va_list ap)
{
    uint8 percentage = 0;
    for (uint16 i = 0, j = 0; format[i]; i++)
    {
        if (format[i] == '%')
        {
            if (!percentage)
            {
                percentage = 1;
                continue;
            }
            else {
                s[j] = '%';
                j++;
            }
        }
        else if (percentage)
        {
            if (format[i] == 'd' || format[i] == 'i')
            {
                int32 d = va_arg(ap, int32);
                char buf[32];
                int_to_string(d, buf);
                for (uint16 k = 0; buf[k]; k++)
                {
                    s[j] = buf[k];
                    j++;
                }
            }
            else if (format[i] == 'f')
            {
                double f = va_arg(ap, double);
                char buf[32];
                float_to_string(f, buf);
                for (uint16 k = 0; buf[k]; k++)
                {
                    s[j] = buf[k];
                    j++;
                }
            }
            else if (format[i] == 'x')
            {
                int32 x = va_arg(ap, int32);
                char buf[32];
                hex_to_string(x, buf);
                for (uint16 k = 0; buf[k]; k++)
                {
                    s[j] = buf[k];
                    j++;
                }
            }
            else if (format[i] == 'c')
            {
                char c = va_arg(ap, uint32);
                s[j] = c;
                j++;
            }
            else if (format[i] == 's')
            {
                char* str = va_arg(ap, char*);
                for (uint16 k = 0; str[k]; k++)
                {
                    s[j] = str[k];
                    j++;
                }
            }
            else 
            {
                s[j] = format[i];
                j++;
            }
        }
        else 
        {
            s[j] = format[i];
            j++;
        }
        percentage = 0;
    }
}