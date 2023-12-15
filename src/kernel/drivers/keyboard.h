#pragma once
#include "../common.h"

#define SC_W 0x11
#define SC_A 0x1E
#define SC_S 0x1F
#define SC_D 0x20
#define SC_SPACE 0x39
#define SC_ENTER 0x1C
#define SC_ESCAPE 1

void init_keyboard();
void destruct_keyboard();
void set_keyboard_callback(void (*callback)(uint8 scancode, char* ascii, uint8 released));
uint8 key_released(uint8 scancode);
void update_key_flags();

uint8 key_press(uint8 scancode);
uint8 key_repeat(uint8 scancode);
uint8 key_release(uint8 scancode);
uint8 key_none(uint8 scancode);