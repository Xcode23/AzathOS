; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
; Declare a multiboot header.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
; Allocate stack.
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 
; Entry point function.
section .text
global _start:function (_start.end - _start)
_start:
 
	; Setup stack.
	mov esp, stack_top
 
	; Call crti.
	extern _init
	call _init

	; Enter the high-level kernel.
	extern kernel_main
	call kernel_main

	; Start infinite loop if kernel returns.
	cli
.hang:	hlt
	jmp .hang
.end:
