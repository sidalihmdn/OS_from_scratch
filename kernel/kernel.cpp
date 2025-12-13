#include <drivers/screen.h>
#include <boot/multiboot_helpers.h>
#include <libc/string.h>
#include <cpu/int.h>
#include <cpu/pic.h>
#include <drivers/keyboard.h>
#include <drivers/display/console.h>
#include <drivers/display/vbe.h>
#include <kernel/mem/heap.h> 
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <kernel/panic.h>
#include <tests/libc_test.h>
#include <drivers/clock/clock.h>

#if DEBUG
//some debug code
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

extern "C" void load_gdt();
static uint32_t total_memory;
void print_memory_map(multiboot_info_t* mb_info);
void print_mem(mmap_entry_t* entry, void* context);

void test_timer_callback(){
    printk("Timer callback\n");
}

extern "C" void kernel_main(multiboot_info_t* mb_info){
    multiboot_info_t* multiboot_info = mb_info;
    total_memory = multiboot_get_total_memory(multiboot_info);
    load_gdt();  // Load our own GDT before anything else
    set_idt();
    init_exceptions();
    init_keyboard();
    clean_screen();
    init_pmm(multiboot_info);
    init_vmm(multiboot_info);
    init_heap();
    console_init(multiboot_info);
    init_clock();

    rtc_time_t time;
    clock_get_date_time(&time);
    printk("Current time: %d:%d:%d\n", (int)time.hour, (int)time.minute, (int)time.second);
    
    
    
    // printk("Welcome to OS from Scratch!\n");
    // printk("Type 'help' for commands.\n");
    // printk("os > ");

    char buffer[256];
    asm volatile("xchg %bx, %bx");
    for(;;){
        if (get_input_buffer(buffer, 256)) {
            if (strcmp(buffer, (char*)"help") == 0) {
                printk("Available commands:\n");
                printk("  help  - Show this help\n");
                printk("  clear - Clear screen\n");
                printk("  mem   - Show memory map\n");
                printk("  test  - Run libc tests\n");
            } else if (strcmp(buffer, (char*)"clear") == 0) {
                clean_screen();
            } else if (strcmp(buffer, (char*)"mem") == 0) {
                print_memory_map(mb_info);
            }else if (strcmp(buffer, (char*)"test") == 0) {
                run_libc_tests();
            }else if (len(buffer) > 0) {
                printk("Unknown command: %s\n", buffer);
            }

            printk("os > ");
        }
        vbe_swap_buffer();
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
