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
 
	; Setup GDT.
	lgdt [gdt_descriptor] ; loads gdt described at gdt_descriptor into GDTR
	jmp CODE_SEG_OFFSET:.gdt_setup_end ; far jump loads CODE_SEG_OFFSET in CS
.gdt_setup_end:

	; Enter the high-level kernel.
	extern kernel_main
	call kernel_main

	; Start infinite loop if kernel returns.
	cli
.hang:	hlt
	jmp .hang
.end:

; GDT
gdt_start:

gdt_null: ; first gdt entry should always be null
	dq 0x0

gdt_code: ; gdt entry for code segment
	dw 0xFFFF 		; segment limit
	dw 0x0    		; segment base
	db 0x0	  		; segment base
	db 0b10011010 	; flags bits 41 for RX if code, 43 for code, 44 for code and data, and 47 for valid sector
	db 0b11001111 	; lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
	db 0x0    		; segment base

gdt_data: ; gdt entry for data segment
	dw 0xFFFF 		; segment limit
	dw 0x0    		; segment base
	db 0x0	  		; segment base
	db 0b10010010 	; flags bits 41 for RW if data, 44 for code and data, and 47 for valid sector
	db 0b11001111 	; lower 4 bits are limit, higher 4 bits are flags 54 for protected mode, and 55 for segments bound to 4k
	db 0x0    		; segment base

gdt_end:

gdt_descriptor:
	dw gdt_end-gdt_start-1
	dd gdt_start

CODE_SEG_OFFSET equ gdt_code - gdt_start
DATA_SEG_OFFSET equ gdt_data  - gdt_start