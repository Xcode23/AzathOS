/* Declare constants for the multiboot header. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
//Declare a multiboot header.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
 
//Allocate stack.
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:
 
//Entry point function.
.section .text
.global _start
.type _start, @function
_start:

	//Setup stack
	mov $stack_top, %esp
 
	//Call crti.
	call _init
	
	//Setup GDT


	//Enter the high-level kernel.
	call kernel_main
 
	//Start infinite loop if kernel returns
	cli
1:	hlt
	jmp 1b
 
//Set the size of the _start symbol.
.size _start, . - _start
