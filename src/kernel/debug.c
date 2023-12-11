#include "debug.h"
#include "io/ports.h"
#include <stdarg.h>

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

void dbg_printf(char* format, ...)
{
#if ENABLE_DEBUG
    va_list arg_ptr;
    va_start(arg_ptr, format);
    char buf[256] = { 0 };
    vsprintf(buf, format, arg_ptr);
    dbg_puts(buf);
    va_end(arg_ptr);
#endif
}
