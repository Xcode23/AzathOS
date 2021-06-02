#include <stdint.h>
#include <stdbool.h>

#include "tty.hpp"

extern "C" void kernel_main(void);

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
}