; this function will be used to write to the screen 
; first version of the code is a siple "hello world" programme 

print_message:
    pusha ; push all the registers into the stack
    mov ah, 0x0e
    jmp start_function ; jmp to the section
start_function:
    mov al, [bx] ; mov the content of the adress stored in bx in al
    cmp al, 0    ; compare the value with O
    je end_function ; if it's equals 0 we jump to the end
    int 0x10 ; call the interrupt (so xe print the content of al in the screen)
    inc bx ; increment bx
    jmp start_function ; loop

end_function:
    popa ; pop all the registers from the stack
    ret  ; return to the adress from which the call instruction is executed