#include "timer.h"
#include "../interrupts/idt.h"
#include "video.h"
#include "../interrupts/irq.h"
#include "../util/list.h"

volatile void(*timer_callback)() = 0;
volatile uint32 tick = 0;
static List events;
static uint8 initialized = 0;

typedef struct {
    uint32 ticks_number;
    void(*event)();
} Event;

void set_timer_callback(void (*callback)()) {
    timer_callback = callback;
}

volatile static void handler(regs *r)
{
    if (!initialized)
        return;
    timer_callback();
    tick++;

    for (list_elem* iter = events.begin; iter != NULL;)
    {
        Event* e = iter->data;
        if (e->ticks_number == 0)
        {
            void(*callback)() = e->event;
            list_elem* next_iter = iter->next;
            free(list_remove_by_elem(&events, iter));
            iter = next_iter;
            callback();
        }
        else 
        {
            e->ticks_number--;
            iter = iter->next;
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
    initialized = 1;
}

static void delete_event(Event* data) {
    free(data);
}

void destruct_timer()
{
    initialized = 0;
    //dbg_printf("Destructing timer\n");
    list_destructor(&events, delete_event);
    //dbg_printf("Destructed\n");
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