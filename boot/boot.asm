[org 0x7c00] ; setting the adress offset
KERNEL_ADDR equ 0x7ef0
BOOT_DISK : db 0
mov [BOOT_DISK], dl

; in this part we load the kernel from the disk
xor ax, ax
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

mov bx, KERNEL_ADDR
mov dh, 20

mov ah, 0x02
mov al, dh

mov ch, 0x00
mov dh, 0x00
mov cl, 0x02

mov dl, [BOOT_DISK]
int 0x13

; switching to text mode and clearing the screen
mov ah, 0x00
mov al, 0x03
int 0x10

; entering the protected mode
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
cli
lgdt[gdt_descriptor]
mov eax, cr0
or al, 0x01
mov cr0, eax
jmp CODE_SEG:_main32

[bits 32]
_main32:
	;set up segment registers again
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	;set up the stack
	mov ebp, 0x90000
	mov esp, ebp

	;jump to kernel location
	jmp KERNEL_ADDR
	

gdt_start:
gdt_null:
	dd 0x00
	dd 0x00
gdt_code:
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_data:
	dw 0xFFFF
	dw 0x00
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00
gdt_end:
gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start
    
; zero padding the rest of the segement size (512 bytes)
times 510-($-$$) db 0
dw 0xaa55 ; the magic number (so the bios know that this is a boot partition)