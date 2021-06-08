#include <stdint.h>
#include <stdbool.h>

#include "tty.hpp"
#include "interrupts.hpp"
#include "PIC.hpp"
#include "kernel.hpp"
#include "GDT.hpp"

extern "C" void kernel_main(void);
static bool is_protected();

static bool is_protected(){
	uint16_t status = 0;
	asm("smsw %0"
			:"=r"(status)
			:
			:);
	return status & 1;
}
 
void kernel_main(void){
	int counter = 0;
	gdt::setup_gdt();
	gdt::load_gdt();
	terminal_initialize();
	terminal_writestring("Hello, kernel World!\n");

	if (is_protected())
		terminal_writestring("The Kernel is in Protected Mode\n");
	else
		terminal_writestring("The Kernel is in Real Mode\n");
	setup_idt();
	load_idt();
	pic_init();
	test_idt();
	//counter = 10/counter;
	asm("sti");
	while(1);
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

void panic() { //TODO: improve panic
	terminal_writestring("PANIC");
	asm volatile("cli; hlt");
}