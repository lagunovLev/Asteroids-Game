#include "keyboard.h"
#include "../interrupts/idt.h"
#include "../interrupts/irq.h"

void (*keyboard_callback)(uint8 scancode, char* ascii, uint8 released);

uint8* key_flags;
uint8* second_key_flags;

static void handler(regs *r) {
    uint8 scancode = inb(0x60);
    uint8 kreleased = key_released(scancode);
    if (kreleased) 
    {
        scancode -= 0x80;
        key_flags[scancode] = 0;
    }
    else key_flags[scancode] = 1;
    keyboard_callback(scancode, "#", kreleased); // TODO ascii code
}

void init_keyboard() {
    irq_install_handler(1, handler); 
    key_flags = malloc(128);
    second_key_flags = malloc(128);
    memset(key_flags, 0, 128);
    memset(second_key_flags, 0, 128);
}

void set_keyboard_callback(void (*callback)(uint8 scancode, char* ascii, uint8 released))
{
    keyboard_callback = callback;
}

uint8 key_released(uint8 scancode)
{
    return scancode & 0x80;
}

void update_key_flags()
{
    memcpy(second_key_flags, key_flags, 128);
}

uint8 key_press(uint8 scancode)
{
    return key_flags[scancode] && !second_key_flags[scancode];
}

uint8 key_repeat(uint8 scancode)
{
    return key_flags[scancode] && second_key_flags[scancode];
}

uint8 key_release(uint8 scancode)
{
    return !key_flags[scancode] && second_key_flags[scancode];
}

uint8 key_none(uint8 scancode)
{
    return !key_flags[scancode] && !second_key_flags[scancode];
}
