#include "IO.hpp"
#include "tty.hpp"
#include "kernel.hpp"
#include "PIC.hpp"
#include "keyboard.hpp"

void keyboard_irq_handler() {
	auto scan_code = inb(0x60);
	char str[256];
	itoa(scan_code, str);
	prints(str);
	prints("\n");
	pic_send_eoi(1);
}