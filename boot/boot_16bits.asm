[bits 16]
_main16:
	;save boot disk number
	mov [BOOT_DISK], dl

	;set up segment registers
	cli
	mov ax, 0x00
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00
	sti
    
    
    ; the result of these calls is stored in memory, for the kernel to fetch later
	call memory_detection
	
	call upper_mem_map
	
	;clear the screen
	mov ah, 0x00
	mov al, 0x03
	int 0x10

	;read 50 sectors at kernel location
	mov bx, KERNEL_LOCATION
	mov dh, 60
	call disk_read

	; WHATEVER IS PUT INTO BX HERE WILL BE WRITTEN INTO THE MEMSIZE VARIABLE!
	; Can be adapted to pass any information to the kernel without storing it in memory
	call detect_cont_memory
	
	jmp enter_protected_mode