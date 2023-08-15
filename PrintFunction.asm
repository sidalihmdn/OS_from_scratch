; this function will be used to write to the screen 
; first version of the code is a siple "hello world" programme 

mov ah, 0x0e

mov al, 'h'
int 0x10
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10

jmp $
