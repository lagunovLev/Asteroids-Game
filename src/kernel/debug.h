#pragma once
#include "io/ports.h"
#include <stdarg.h>
#include "common.h"
#include "util/list.h"
#include "drivers/video.h"

#define ENABLE_DEBUG 1
#define PAGE_WIDTH 40
#define PAGE_HEIGHT 23

typedef struct TextPage
{
    char text[PAGE_WIDTH][PAGE_HEIGHT];
} TextPage;

void init_debug();
void destruct_debug();

void dbg_putc(char c);
void dbg_puts(char* str);
void dbg_printf(char* format, ...);

void view_logs(uint8 start);