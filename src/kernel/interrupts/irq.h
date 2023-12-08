#pragma once
#include "idt.h"

void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();
void irq_install_handler(uint8 irq, void (*handler)(regs *r));
void irq_uninstall_handler(uint8 irq);
void irq_remap();
void irq_install();
void irq_handler(regs *r);