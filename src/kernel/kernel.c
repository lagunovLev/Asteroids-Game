#include "drivers/video.h"
#include "io/ports.h"
#include "math.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/irq.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"

char* str = "";

static void timer_callback();
static void kb_callback(uint8 scancode, char* ascii, uint8 released);
static void calculate_delay(uint32 delay, uint32 start_tick, uint32 end_tick);

void kmain()
{
    idt_install();
    isrs_install();
    irq_install();
    asm volatile("sti");
    set_timer_callback(timer_callback);
    init_timer(50);
    set_keyboard_callback(kb_callback);
    init_keyboard();
    malloc_init();
    
    uint32 i = 0;
    while (1)
    {
        uint32 start_tick = tick;

        clearScreen(0x00);

        if (key_none(72)) 
            str = "None";
        if (key_press(72)) 
            str = "Press";
        if (key_release(72)) 
            str = "Release";
        if (key_repeat(72)) 
            str = "Repeat";
        update_key_flags();

        putString(0, 0, 0x65, "Ticks: %d\n\rKeyPressed: %s", i, str);
        drawFillRect(30, 60, 30, 20, 0x65);
        i++;
        flip();

        uint32 end_tick = tick;
        calculate_delay(1, start_tick, end_tick);
    }
}

static void calculate_delay(uint32 delay, uint32 start_tick, uint32 end_tick)
{
    if (end_tick - start_tick < delay)
    {
        uint32 time_last = end_tick - start_tick;
        wait(delay - time_last);
    }
}

static void timer_callback()
{
    
} 

static void kb_callback(uint8 scancode, char* ascii, uint8 released)
{

}