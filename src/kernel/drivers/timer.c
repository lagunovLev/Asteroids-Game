#include "timer.h"
#include "../interrupts/idt.h"
#include "video.h"
#include "../interrupts/irq.h"
#include "../util/list.h"

volatile void(*timer_callback)() = 0;
volatile uint32 tick = 0;
static List events;

typedef struct {
    uint32 ticks_number;
    void(*event)();
} Event;

void set_timer_callback(void (*callback)()) {
    timer_callback = callback;
}

volatile static void handler(regs *r)
{
    timer_callback();
    tick++;

    uint32 j = 0;
    for (list_elem* i = events.begin; i != NULL; i = i->next)
    {
        Event* e = (Event*)i->data;
        if (e->ticks_number == 0)
        {
            void(*callback)() = e->event;
            i = i->next;
            free(list_remove(&events, j));
            callback();
        }
        else 
        {
            e->ticks_number--;
            j++;
        }
    }
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

    list_constructor(&events);
}

void delete_event(Event* data) {
    free(data);
}

void destruct_timer()
{
    list_destructor(&events, delete_event);
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

void add_event(void(*event)(), uint32 ticks) {
    Event* e = malloc(sizeof(Event));
    e->ticks_number = ticks;
    e->event = event;
    list_push_back(&events, e);
}