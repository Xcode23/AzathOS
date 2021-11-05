#include <stdint.h>
#include <stdbool.h>

#include "tty.hpp"
#include "interrupts.hpp"
#include "PIC.hpp"
#include "kernel.hpp"
#include "GDT.hpp"
#include "string.hpp"

extern "C" void kernel_main(/*multiboot_info_t* mbd*/);
extern "C" void setup_idt();
static bool is_protected();

static bool is_protected(){
	uint16_t status = 0;
	asm("smsw %0"
			:"=r"(status)
			:
			:);
	return status & 1;
}
 
void kernel_main(/*multiboot_info_t* mbd*/){
	gdt::setup_gdt();
	gdt::load_gdt();
	terminal_initialize();
	prints("Hello, kernel World!\n");

	if (is_protected())
		prints("The Kernel is in Protected Mode\n");
	else
		prints("The Kernel is in Real Mode\n");
	setup_idt();
	load_idt();
	pic_init();
	test_idt();
	asm("sti");

	while(1);
}

void panic() { //TODO: improve panic
	prints("PANIC");
	asm volatile("cli; hlt");
}