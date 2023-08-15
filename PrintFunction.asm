; this function will be used to write to the screen 
; first version of the code is a siple "hello world" programme 

print_message:
    pusha
    mov ah, 0x0e
    jmp start_function
start_function:
    mov al, [bx]
    cmp al, 0
    je end_function
    int 0x10
    inc bx
    jmp start_function

end_function:
    popa
    ret