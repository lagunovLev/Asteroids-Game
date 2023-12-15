#pragma once
#include "../common.h"

extern volatile uint32 tick;

void set_timer_callback(void (*callback)());
void init_timer(uint32 freq);
void destruct_timer();
void wait(uint32 ticks);

void add_event(void(*event)(), uint32 ticks);