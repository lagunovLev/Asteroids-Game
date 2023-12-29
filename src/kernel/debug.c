#include "debug.h"
#include "drivers/font.h"
#include "drivers/keyboard.h"

List pages;

void init_debug()
{
    list_constructor(&pages);
    TextPage* page = malloc(sizeof(TextPage));
    memset(page->text, PAGE_HEIGHT * PAGE_WIDTH, '\0');
    list_push_back(&pages, page);
}

void delete_page(TextPage* page)
{
    free(page);
}

void destruct_debug()
{
    list_destructor(&pages, delete_page);
}

void view_logs(uint8 start)
{
    uint8 run = 1;
    list_elem* i;
    if (start) i = pages.end;
    else i = pages.begin;
    while (run)
    {
        clearScreen(0x00);
        TextPage* page = i->data;
        for (int y = 0; y < PAGE_HEIGHT; y++)
        {
            for (int x = 0; x < PAGE_WIDTH; x++)
            {
                putChar(x * char_width, y * char_height, page->text[x][y], 0x0F);
            }
        }
        putString(0, (PAGE_HEIGHT + 1) * char_height, 0x0F, "Arrows - flip pages\tEnter - exit");
        flip();

        while (1)
        {
            if (key_press(SC_LEFT) && (i->prev != NULL))
            {
                i = i->prev;
                break;
            }
            if (key_press(SC_RIGHT) && (i->next != NULL))
            {
                i = i->next;
                break;
            }
            if (key_press(SC_ENTER))
            {
                run = 0;
                break;
            }
        }
    }
}

inline void dbg_putc(char c)
{
#if ENABLE_DEBUG
    static uint8 x = 0;
    static uint8 y = 0;
    outb(0xE9, c);
    if (c == '\n')
    {
        x = 0;
        y++;
        return;
    }
    if (c == '\t')
    {
        x += 4;
        return;
    }
    if (x >= PAGE_WIDTH)
    {
        x = 0;
        y++;
    }
    if (y >= PAGE_HEIGHT)
    {
        TextPage* page = malloc(sizeof(TextPage));
        memset(page->text, PAGE_HEIGHT * PAGE_WIDTH, '\0');
        list_push_back(&pages, page);
        y = 0;
        x = 0;
    }

    ((TextPage*)pages.end->data)->text[x][y] = c;
    x++;
#endif
}

inline void dbg_puts(char* str)
{
#if ENABLE_DEBUG
    while (*str)
    {
        dbg_putc(*str);
        str++;
    }
#endif
}

inline void dbg_printf(char* format, ...)
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
