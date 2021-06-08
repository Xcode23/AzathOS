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

;Interrupt handlers
%macro no_error_code_isr 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword 0        ; push 0 as error code
        push    dword %1       ; push the interrupt number
        jmp     isr_wrapper    ; jump to the common handler
%endmacro

%macro error_code_isr 1
    global interrupt_handler_%1
    interrupt_handler_%1:
        push    dword %1       ; push the interrupt number
        jmp     isr_wrapper    ; jump to the common handler
%endmacro

isr_wrapper:
	pushad
	extern generic_interrupt_handler
	call generic_interrupt_handler
	popad
	add esp, 8						; remove from stack error code and interrupt number
	iret								; TODO: DONT FORGET iret


; generate interrupt handlers
; interrupts 8, 10, 11, 12, 13, 14 and 17 have error codes
%assign i 0
%rep 256
%if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
	error_code_isr i
%else
	no_error_code_isr i
%endif
%assign i i+1
%endrep