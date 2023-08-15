[org 0x7c00] ; setting the adress offset
mov ah, 0x0e ; init the printing interrupt
mov bx, HelloWorld ; storing the adress of the first character of the string in bx

call print_message ; call the printing function (execute the code in the section)


jmp $ ; jump to the current adress so we do not execute what's under

; the string is stored here
HelloWorld:
    db 'Hello world !',0


%include "PrintFunction.asm"

; zero padding the rest of the segement size (512 bytes)
times 510-($-$$) db 0

dw 0xaa55 ; the magic number (so the bios know that this is a boot partition)