#include <stdint.h>

#include "interrupts.hpp"
#include "tty.hpp"
#include "kernel.hpp"
#include "IO.hpp"
#include "keyboard.hpp"
#include "PIC.hpp"
#include "string.hpp"

const uint16_t IDT_SIZE = 256;

/**
 * struct for registers saved in stack in assembly
 * used as a padding parameter to access the 
 * relevant data deeper in the stack 
**/
struct Registers { 
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
} __attribute__((packed));

/**
 * struct for registers saved in stack in assembly
 * used as a padding parameter to access the 
 * relevant data deeper in the stack. 
**/
struct InterruptData { //TODO: add parameters for interrupt handlers
	uint32_t error_code; 
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

struct IDTEntry {
    uint16_t segment_offset_1; // segment offset bits 0..15
    uint16_t selector; // a code segment selector in GDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t segment_offset_2; // segment offset bits 16..31
} __attribute__((packed));

struct IDTDescriptor {
	uint16_t size;
	uint32_t address;
} __attribute__((packed));

IDTEntry idt[IDT_SIZE];

IDTDescriptor idt_descriptor = {sizeof(IDTEntry) * IDT_SIZE - 1, reinterpret_cast<uint32_t>(&idt)}; //TODO: Add comments

extern "C" void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd);
extern "C" void setup_isr(uint8_t intr_num, uintptr_t handler);
void stub_isr(uint32_t intr_num);
void exception_handler(uint32_t intr_num, uint32_t error_code);
void hardware_irq_handler(uint32_t intr_num);
void timer_irq_handler();


void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd){
	asm("cli");
	if (intr_num <= 31) { //TODO: improve exception check
		exception_handler(intr_num, intrd.error_code);
	} else if (intr_num == 32) {
		timer_irq_handler();
	} else if (intr_num == 33) {
		keyboard_irq_handler();
	} else if (intr_num >= 34 && intr_num <= 47) {//PIC has 16 interrupts for hardware from 32-47
		hardware_irq_handler(intr_num);
	} else {
		stub_isr(intr_num);
	}
	asm("sti");
}

void stub_isr(uint32_t intr_num) {
	char str[256];
	itoa(intr_num, str);
	prints(str);
	prints("\n");
}

void exception_handler(uint32_t intr_num, uint32_t error_code) {
	char str[256];
	itoa(intr_num, str);
	prints(str);
	if (error_code != 0) {
		char str2[256];
		itoa(error_code, str2);
		prints("   ");
		prints(str2);
	}
	prints("\n");
	panic();
}

void hardware_irq_handler(uint32_t intr_num) {
	char str[256];
	itoa(intr_num, str);
	prints(str);
	prints("\n");
	pic_send_eoi(intr_num-32);
}

void timer_irq_handler() {
	pic_send_eoi(0);
}

void setup_isr(uint8_t intr_num, uintptr_t handler) {
	idt[intr_num].segment_offset_1 = handler & 0xFFFF;
	idt[intr_num].segment_offset_2 = handler >> 16;
	idt[intr_num].zero = 0;
	idt[intr_num].selector = 8; //code segment gdt offset
	idt[intr_num].type_attr = 0b10001111; //bits 0-3 specify gate type, with all set for 32-bit trap-gate, 8th bit for present interrupt
}

void load_idt() {
	asm("lidt %0" :: "m" (idt_descriptor));
}

void test_idt() {
	asm("int $60");
	asm("int $90");
}