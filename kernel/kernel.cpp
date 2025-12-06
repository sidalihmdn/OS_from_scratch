#include "../includes/drivers/screen.h"
#include "../includes/boot/multiboot_helpers.h"
#include "../includes/libc/string.h"
#include "../includes/cpu/int.h"
#include "../includes/cpu/pic.h"
#include "../includes/drivers/keyboard.h"
#include "../includes/kernel/mem/mem.h" 
#include "../includes/kernel/mem/pmm.h"
#include "../tests/libc_test.h"
#include "../includes/kernel/panic.h"

#if DEBUG
//some debug code
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

extern "C" void load_gdt();
void print_memory_map(multiboot_info_t* mb_info);
void print_mem(mmap_entry_t* entry, void* context);

extern "C" void kernel_main(multiboot_info_t* mb_info){
    load_gdt();  // Load our own GDT before anything else
    set_idt();
    init_exceptions();
    init_keyboard();
    //init_pmm();
    //init_mem();

    clean_screen();
    printk("Welcome to OS from Scratch!\n");
    printk("Type 'help' for commands.\n");
    printk("os > ");
    print_memory_map(mb_info);
    init_pmm(mb_info);
    char buffer[256];
    
    for(;;){
        if (get_input_buffer(buffer, 256)) {
            if (strcmp(buffer, (char*)"help") == 0) {
                printk("Available commands:\n");
                printk("  help  - Show this help\n");
                printk("  clear - Clear screen\n");
                printk("  echo  - Echo text\n");
                printk("blabla\n");
            } else if (strcmp(buffer, (char*)"clear") == 0) {
                clean_screen();
            } else if (buffer[0] == 'e' && buffer[1] == 'c' && buffer[2] == 'h' && buffer[3] == 'o' && buffer[4] == ' ') {
                printk(buffer + 5);
                printk("\n");
            } else if (len(buffer) > 0) {
                printk("Unknown command: %s\n", buffer);
            }
            
            printk("os > ");
        }
    }
}

void print_memory_map(multiboot_info_t* mb_info){
    for(mmap_entry_t* entry = (mmap_entry_t*)mb_info->mmap_addr; entry < (mmap_entry_t*)(mb_info->mmap_addr + mb_info->mmap_length); entry++){
        printk("base : %s ", ptr_to_hex(entry->addr));
        printk("\tlength : %s ", ptr_to_hex(entry->len));
        printk("\ttype : %s\n", int2String(entry->type));
    }
}

void print_mem(mmap_entry_t* entry, void* context){
    printk("base : %s ", ptr_to_hex(entry->addr));
    printk("\tlength : %s ", ptr_to_hex(entry->len));
    printk("\ttype : %s\n", int2String(entry->type));
}
