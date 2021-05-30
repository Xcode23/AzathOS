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
	lgdt (gdt_descriptor) //loads gdt described at gdt_descriptor into GDTR
	jmp $CODE_SEG_OFFSET,$gdt_setup_end //far jump loads CODE_SEG_OFFSET in CS
gdt_setup_end:

	//Enter the high-level kernel.
	call kernel_main
 
	//Start infinite loop if kernel returns
	cli
1:	hlt
	jmp 1b


//GDT
gdt_start:

gdt_null: //first gdt entry should always be null
	.quad 0x0

gdt_code: //gdt entry for code segment
	.short 0xFFFF //segment limit
	.short 0x0    //segment base
	.byte   0x0	   //segment base
	.byte   0b10011010 //flags bits 41 for RX if code, 43 for code, 44 for code and data, and 47 for valid sector
	.byte   0b11001111 //lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
	.byte   0x0    //segment base

gdt_data: //gdt entry for data segment
	.short 0xFFFF //segment limit
	.short 0x0    //segment base
	.byte   0x0	   //segment base
	.byte   0b10010010 //flags bits 41 for RW if data, 44 for code and data, and 47 for valid sector
	.byte   0b11001111 //lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
	.byte   0x0    //segment base

gdt_end:

gdt_descriptor:
	.short gdt_end-gdt_start-1
	.long gdt_start

.equ CODE_SEG_OFFSET, gdt_code - gdt_start
.equ DATA_SEG_OFFSET, gdt_data  - gdt_start
 
//Set the size of the _start symbol.
.size _start, . - _start
