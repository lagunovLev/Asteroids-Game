#include "debug.h"
#include "drivers/font.h"
#include "drivers/keyboard.h"

List pages;
volatile char* current_msg = 0;
uint16 msg_i = 0;
uint8 new_line = 0;

void init_debug()
{
    list_constructor(&pages);
    TextPage* page = malloc(sizeof(TextPage));
    for (int i = 0; i < PAGE_WIDTH; i++)
        memset(page->text[i], 0, PAGE_HEIGHT);
    list_push_back(&pages, page);
    current_msg = malloc(PAGE_WIDTH);
    clear_msg();
}

void clear_msg()
{
    msg_i = 0;
    memset(current_msg, ' ', PAGE_WIDTH);
    current_msg[PAGE_WIDTH - 1] = '\0';
}

void write_msg(char ch)
{
    if (msg_i >= PAGE_WIDTH - 2 || new_line)
    {
        clear_msg();
        new_line = 0;
    }
    current_msg[msg_i] = ch;
    msg_i++;
}

void delete_page(TextPage* page)
{
    free(page);
}

void destruct_debug()
{
    list_destructor(&pages, delete_page);
    free(current_msg);
}

void view_logs(uint8 start)
{
    uint8 run = 1;
    list_elem* i;
    uint32 cur_page;
    if (start) {
        i = pages.end;
        cur_page = pages.size - 1;
    }
    else { 
        i = pages.begin;
        cur_page = 0;
    }

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
        putString(0, (PAGE_HEIGHT + 1) * char_height, 0x0F, "Arrows - flip pages  Enter - exit  %d", cur_page);
        flip();

        uint8 r = 1;
        while (r)
        {
            wait(2);
            if (key_press(SC_LEFT) && (i->prev != NULL))
            {
                i = i->prev;
                r = 0;
                cur_page--;
            }
            if (key_press(SC_RIGHT) && (i->next != NULL))
            {
                i = i->next;
                r = 0;
                cur_page++;
            }
            if (key_press(SC_ENTER))
            {
                run = 0;
                r = 0;
            }
            update_key_flags();
        }
    }
}

inline void dbg_putc(char c)
{
#if ENABLE_DEBUG
    static uint8 x = 0;
    static uint8 y = 0;
    outb(0xE9, c);
    if (c == "\n")
        new_line = 1;
    else
        write_msg(c);
    if (c == '\n')
    {
        x = 0;
        y++;
        return;
    }
    if (c == '\t')
    {
        x += 4;
        msg_i += 4;
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
        for (int i = 0; i < PAGE_WIDTH; i++)
            memset(page->text[i], 0, PAGE_HEIGHT);
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
