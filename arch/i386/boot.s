; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
; Declare a multiboot header.
section .multiboot.data
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
; Allocate stack.
section .bootstrap_stack
align 16
stack_bottom:
times 16384 db 0 ; 16 KiB
stack_top:

section .bss
align 4096
global boot_page_directory
boot_page_directory:
	resb 4096
global boot_page_table1
boot_page_table1:
	resb 4096
align 4
multiboot_info:
	resd 1
; Further page tables may be required if the kernel grows beyond 3 MiB.

section .multiboot.text
global _start
_start:

	; Save multiboot info
	mov [multiboot_info], ebx 

	; Setup stack
	mov esp, (stack_top - 0xC0000000)

	; Setup higher half kernel.
	extern higher_half_setup
	mov eax, higher_half_setup - 0xC0000000
	call eax

	; Jump to higher half entry point
	lea ecx, _higher_half_start
	jmp ecx
.end:
 
; Entry point function.
section .text
_higher_half_start:

	; Unmap the identity mapping as it is now unnecessary. 
	mov [boot_page_directory + 0], dword 0

	; Reload crc3 to force a TLB flush so the changes to take effect.
	mov ecx, cr3
	mov cr3, ecx
 
	; Setup stack.
	mov esp, stack_top
 
	; Call crti.
	extern _init
	call _init

	; Enter the higher half kernel.
	extern kernel_main
	push dword [multiboot_info] ; pass the multiboot info struct address to kernel_main as parameter for the memory size
	call kernel_main

	; Start infinite loop if kernel returns.
	cli
.hang:	hlt
	jmp .hang
.end:
