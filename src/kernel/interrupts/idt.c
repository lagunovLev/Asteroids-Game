#include "../common.h"

typedef struct idt_entry			// IDT structure
{
	uint16 base_lo;
	uint16 sel;	
	uint8 always0;	
	uint8 flags;	
	uint16 base_hi;
} __attribute__((packed)) idt_entry;

typedef struct idt_ptr				// IDT poiner
{
	uint16 limit;
	uint32 base;
} __attribute__((packed)) idt_ptr;


idt_entry idt[256] = { 0 };
idt_ptr idtp;

void idt_load();		// ---> interrupt.asm

void idt_set_gate(uint8 num, uint32 base, uint16 sel, uint8 flags)
{
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (base >> 16) & 0xFFFF;

	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

void idt_install()
{
	idtp.limit = (sizeof(idt_entry) * 256) - 1;
	idtp.base = &idt;

	/* Add any new ISRs to the IDT here using idt_set_gate */

	/* Points the processor's internal register to the new IDT */
	idt_load();
}