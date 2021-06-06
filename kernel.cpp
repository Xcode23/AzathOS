#include <stdint.h>
#include <stdbool.h>

#include "tty.hpp"
#include "interrupts.h"

extern "C" void kernel_main(void);
void itoa (unsigned int number, char* str);
extern "C" void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd);
void setup_idt(void);
void load_idt(void);
void test_idt(void);

static bool is_protected(){
	uint16_t status = 0;
	__asm__("smsw %0"
			:"=r"(status)
			:
			:);
	return status & 1;
}
 
void kernel_main(void){
	terminal_initialize();
	terminal_writestring("Hello, kernel World!\n");

	if (is_protected())
		terminal_writestring("The Kernel is in Protected Mode\n");
	else
		terminal_writestring("The Kernel is in Real Mode\n");
	setup_idt();
	load_idt();
	test_idt();
}

void generic_interrupt_handler(Registers regs, uint32_t intr_num, InterruptData intrd){
	terminal_writestring("Success\n");
	char str[256];
	itoa(intr_num, str);
	terminal_writestring(str);
	terminal_writestring("\n");
	if (intr_num == 60) {
		terminal_writestring("IRQ60\n");
	}
}



void itoa (unsigned int number, char* str) {
	unsigned int temp[256];
	int i = 0;
	while (number > 10) {
		temp[i++] = number % 10;
		number = number / 10;
	}
	temp[i] = number % 10;
	int j = 0;
	while (i >= 0) {
		str[j++]='0' + temp[i--];
	}
	str[j]=0;
}

void setup_idt(void) {
	idt[60].segment_offset_1 = reinterpret_cast<uintptr_t>(interrupt_handler_60) & 0xFFFF;
	idt[60].segment_offset_2 = reinterpret_cast<uintptr_t>(interrupt_handler_60) >> 16;
	idt[60].zero = 0;
	idt[60].selector = 8; //code segment gdt offset
	idt[60].type_attr = 0b10001110; //TODO: improve
}

void load_idt(void) {
	__asm__("lidt %0" :: "m" (idt_descriptor));
}

void test_idt(void) {
	__asm__("sti");
	__asm__("int $60");
	__asm__("cli");
}

