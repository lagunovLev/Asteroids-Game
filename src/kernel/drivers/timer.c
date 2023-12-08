#include "timer.h"
#include "../interrupts/idt.h"
#include "video.h"
#include "../interrupts/irq.h"

void(*timer_callback)() = 0;
uint32 tick = 0;

void set_timer_callback(void (*callback)()) {
    timer_callback = callback;
}

static void handler(regs *r)
{
    timer_callback();
    tick++;
}

void init_timer(uint32 freq)
{
    // Для начала регистрируем наш callback
    irq_install_handler(0, handler);

    // Значение, сообщаемое в PIT
    uint32 divisor = 1193180 / freq;

    // Послать команду
    outb(0x43, 0x36); 

    // Значение посылается в два этапа
    uint8 l = (uint8)(divisor & 0xFF);
    uint8 h = (uint8)((divisor >> 8) & 0xFF);

    // Посылаем на порт данные
    outb(0x40, l);
    outb(0x40, h);
}

void wait(uint32 ticks)
{
    uint32 start_tick = tick;
    while (1)
    {
        if (tick - start_tick >= ticks)
            return;
        if (tick >= 4294967295 - 20)
        {
            uint32 diff = tick - start_tick;
            start_tick = 0;
            tick = diff;
        }
    }
}