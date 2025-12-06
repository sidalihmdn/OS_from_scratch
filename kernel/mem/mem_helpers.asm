global load_page_directory
load_page_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get the pointer argument
    mov cr3, eax        ; Load CR3
    mov esp, ebp
    pop ebp
    ret
global enable_paging
enable_paging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000  ; Set bit 31 (PG)
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret