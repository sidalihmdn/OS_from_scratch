; Defined in int.cpp
[extern isr_handler]
[extern irq_handler]

global cli
cli:
    cli
    ret

; Common ISR code
isr_common_stub:
    pusha               ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    
    mov ax, ds          ; Lower 16-bits of eax = ds.
    push eax            ; save the data segment descriptor
    
    mov ax, 0x10        ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call isr_handler
    
    pop eax             ; reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                ; Pops edi,esi,ebp...
    add esp, 8          ; Cleans up the pushed error code and pushed ISR number
    iret                ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Common IRQ code
irq_common_stub:
    pusha               ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
    
    mov ax, ds          ; Lower 16-bits of eax = ds.
    push eax            ; save the data segment descriptor
    
    mov ax, 0x10        ; load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call irq_handler
    
    pop eax             ; reload the original data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                ; Pops edi,esi,ebp...
    add esp, 8          ; Cleans up the pushed error code and pushed ISR number
    iret                ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

%macro isr_no_err_stub 1
isr_stub_%+%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro isr_err_stub 1
isr_stub_%+%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

%macro irq_stub 2
irq_stub_%+%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

irq_stub 0, 32
irq_stub 1, 33
irq_stub 2, 34
irq_stub 3, 35
irq_stub 4, 36
irq_stub 5, 37
irq_stub 6, 38
irq_stub 7, 39
irq_stub 8, 40
irq_stub 9, 41
irq_stub 10, 42
irq_stub 11, 43
irq_stub 12, 44
irq_stub 13, 45
irq_stub 14, 46
irq_stub 15, 47

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i
%assign i i+1 
%endrep
    dd irq_stub_0
    dd irq_stub_1
    dd irq_stub_2
    dd irq_stub_3
    dd irq_stub_4
    dd irq_stub_5
    dd irq_stub_6
    dd irq_stub_7
    dd irq_stub_8
    dd irq_stub_9
    dd irq_stub_10
    dd irq_stub_11
    dd irq_stub_12
    dd irq_stub_13
    dd irq_stub_14
    dd irq_stub_15

