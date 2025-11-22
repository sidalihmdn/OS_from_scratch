; kernel_entry.asm
bits 32

global _start     ; <-- make _start visible to the linker
extern main       ; <-- declare main implemented in C/C++

_start:
    call main     ; call your kernel main function
    cli           ; disable interrupts
halt:
    hlt           ; halt CPU
    jmp halt      ; infinite loop
