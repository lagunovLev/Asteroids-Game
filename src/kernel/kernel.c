#include "drivers/video.h"
#include "io/ports.h"
#include "math.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/irq.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "game_logic/game.h"

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
    srand(trand());
    game_init();
    
    while (1)
    {
        uint32 start_tick = tick;

        clearScreen(0x00);
        game_input();
        update_key_flags();
        game_logic();
        game_graphics();
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