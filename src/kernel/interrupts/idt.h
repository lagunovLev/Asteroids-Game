#pragma once
#include "../io/ports.h"
#include "../common.h"

void idt_load();
void idt_set_gate(uint8 num, uint32 base, uint16 sel, uint8 flags);
void idt_install();

typedef struct regs
{
    uint32 gs, fs, es, ds;      /* pushed the segs last */
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    uint32 int_no, err_code;    /* our 'push byte #' and ecodes do this */
    uint32 eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
} __attribute__ ((packed)) regs;