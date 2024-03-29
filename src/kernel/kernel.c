#include "drivers/video.h"
#include "io/ports.h"
#include "math.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/irq.h"
#include "drivers/timer.h"
#include "drivers/keyboard.h"
#include "game_logic/game.h"
#include "game_logic/scene.h"
#include "game_logic/menu.h"
#include "debug.h"

static void timer_callback();
static void kb_callback(uint8 scancode, char* ascii, uint8 released);
static void calculate_delay(uint32 delay, uint32 start_tick, uint32 end_tick);

uint8 run = 1;
uint8 run_current_scene = 1;

void kmain()
{
    idt_install();
    isrs_install();
    irq_install();
    srand(trand());
    malloc_init();
    set_timer_callback(timer_callback);
    init_timer(50);
    set_keyboard_callback(kb_callback);
    init_keyboard();
    init_debug();
    asm volatile("sti");

    clearScreen(0x00);

    struct Scene scene = {
        menu_init,
        menu_input,
        menu_logic,
        menu_graphics,
        menu_destruct,
    };

    while (run)
    {
        scene.init();
        while (run_current_scene)
        {
            uint32 start_tick = tick;

            scene.input();
            update_key_flags();
            scene.logic();
            scene.graphics();
            //dbg_puts("d3");
            flip();
            //dbg_puts("d4");
            uint32 end_tick = tick;
            calculate_delay(1, start_tick, end_tick);
            //dbg_puts("d5");
        }
        scene = scene.destruct();
        run_current_scene = 1;
    }

    view_logs(0);
    asm volatile("cli");
    destruct_timer();
    destruct_debug();
    destruct_keyboard();
    malloc_destruct();
}

static void calculate_delay(uint32 delay, uint32 start_tick, uint32 end_tick)
{
    //dbg_puts("z1");
    if (end_tick - start_tick < delay)
    {
        //dbg_puts("z2");
        uint32 time_last = end_tick - start_tick;
        //dbg_puts("z3");
        wait(delay - time_last);
        //dbg_puts("z4");
    }
}

static void timer_callback()
{
    
} 

static void kb_callback(uint8 scancode, char* ascii, uint8 released)
{

}