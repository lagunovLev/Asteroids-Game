#include "debug.h"
#include "io/ports.h"

void dbg_putc(char c)
{
#if ENABLE_DEBUG
    outb(0xE9, c);
#endif
}

void dbg_puts(char* str)
{
#if ENABLE_DEBUG
    while (*str)
    {
        dbg_putc(*str);
        str++;
    }
#endif
}
