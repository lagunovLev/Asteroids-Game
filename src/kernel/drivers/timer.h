#pragma once
#include "../common.h"

extern uint32 tick;

void set_timer_callback(void (*callback)());
void init_timer(uint32 freq);
void wait(uint32 ticks);