#include "IO.hpp"
#include "tty.hpp"
#include "PIC.hpp"
#include "keyboard.hpp"
#include "string.hpp"

/*
*	Portuguese keyboard, PS/2 Scan Code 1
*/

char scan_code_to_event(uint8_t scan_code);
char inline upcase(char c);

static bool shift = false;
static bool caps_lock = false;
const char* keyboard_line1 = "qwertyuiop";
const char* keyboard_line2 = "asdfghjkl";
const char* keyboard_line3 = "zxcvbnm";

// for later
// typedef struct keyevent {
// 	uint8_t scan_code;
// 	bool shift;
// 	bool caps_lock;
// } keyevent;

char scan_code_to_event(uint8_t scan_code) {
	char result = 0;
	switch(scan_code) {
		case 0x10 ... 0x19:
			result = keyboard_line1[scan_code - 0x10];
			result = upcase(result);
			break;
		case 0x1C:
			result = '\n';
			break;
		case 0x1E ... 0x26:
			result = keyboard_line2[scan_code - 0x1E];
			result = upcase(result);
			break;
		case 0x2A:
			shift = true;
			break;
		case 0x2C ... 0x32:
			result = keyboard_line3[scan_code - 0x2C];
			result = upcase(result);
			break;
		case 0x33:
			if(shift)
				result = ';';
			else
				result = ',';
			break;
		case 0x34:
			if(shift)
				result = ':';
			else
				result = '.';
			break;
		case 0x35:
			if(shift)
				result = '_';
			else
				result = '-';
			break;
		case 0x36:
			shift = true;
			break;
		case 0x39:
			result = ' ';
			break;
		case 0x3A:
			caps_lock = !caps_lock;
			break;
		case 0xAA:
			shift = false;
			break;
		case 0xB6:
			shift = false;
			break;
		default:
			break;
	}
	return result;
}

char inline upcase(char c) {
	if((caps_lock && !shift) || (!caps_lock && shift))
		c = c - 'a' + 'A';
	return c;
}

void keyboard_irq_handler() {
	auto scan_code = inb(0x60);
	char str[2] = "0";
	//xtoa(scan_code, str);
	str[0] = scan_code_to_event(scan_code);
	prints(str);
	pic_send_eoi(1);
}