[org 0x7c00]
KERNEL_ADDR equ 0x1000

; Set up stack and segments first
xor ax, ax
mov es, ax
mov ds, ax
mov bp, 0x8000
mov sp, bp

; BIOS passes drive number in DL, save it
mov [BOOT_DISK], dl

; Debug: Print 'D' then drive number status
mov ah, 0x0e
mov al, 'D'
int 0x10

mov dl, [BOOT_DISK]
cmp dl, 0x80
jge .hard_disk
mov al, 'F' ; Floppy (< 0x80)
jmp .print_drive
.hard_disk:
mov al, 'H' ; Hard Disk (>= 0x80)
.print_drive:
mov ah, 0x0e
int 0x10

; Load Kernel using LBA
mov bx, KERNEL_ADDR
mov dh, 50 ; Load 50 sectors (25KB) - matches current kernel size
call disk_load_lba

; Switch to 32-bit mode
mov ah, 0x00
mov al, 0x03
int 0x10

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

cli
lgdt [gdt_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:init_pm

[bits 16]
disk_load_lba:
    ; Check if LBA extensions are supported
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [BOOT_DISK]
    int 0x13
    jc .extension_error

    ; Prepare Disk Address Packet (DAP) on stack
    push dword 0 ; Upper 32 bits of LBA
    push dword 1 ; Lower 32 bits of LBA (Start at sector 1)
    push word 0x0000 ; Segment
    push word KERNEL_ADDR ; Offset
    xor ax, ax
    mov al, dh
    push ax ; Number of sectors
    push word 0x0010 ; Size of packet (16 bytes)

    ; Call Extended Read
    mov ah, 0x42
    mov dl, [BOOT_DISK]
    mov si, sp
    int 0x13
    jc .read_error

    add sp, 16
    ret

.extension_error:
    mov ah, 0x0e
    mov al, 'X' ; X for Extension Error
    int 0x10
    jmp $

.read_error:
    mov ah, 0x0e
    mov al, 'R' ; R for Read Error
    int 0x10
    
    ; Print Error Code in AH
    mov al, ah
    add al, '0' ; Simple hex to char (only works for 0-9, but good enough for common errors)
    mov ah, 0x0e
    int 0x10
    jmp $

BOOT_DISK: db 0

gdt_start:
gdt_null:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_ADDR
    jmp $

times 510-($-$$) db 0
dw 0xaa55