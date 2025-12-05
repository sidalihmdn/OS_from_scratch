; Multiboot header for GRUB
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; multiboot flags
MAGIC    equ  0x1BADB002        ; magic number
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB stack
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
    ; Set up stack
    mov esp, stack_top

    ; Call kernel main
    extern kernel_main
    push ebx ;multiboot info structure
    call kernel_main

    ; Hang if kernel returns
    cli
.hang:
    hlt
    jmp .hang
.end:
