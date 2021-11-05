

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

global setup_idt
setup_idt:
    push ebp
    mov ebp, esp
    %assign i 0
    %rep 256
        sub esp, 16
        mov [esp + 4], dword interrupt_handler_ %+ i
        mov [esp], byte i ; add to stack(parameters) interrupt number
        extern setup_isr
        call setup_isr
        add     esp, 16 ; remove from stack 1 byte from interrupt number and 4 for isr function pointer
    %assign i i+1
    %endrep
    pop ebp
    ret