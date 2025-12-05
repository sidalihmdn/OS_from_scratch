#include "../includes/drivers/screen.h"
#include "../includes/boot/multiboot.h"
#include "../includes/libc/string.h"
#include "../includes/cpu/int.h"
#include "../includes/cpu/pic.h"
#include "../includes/drivers/keyboard.h"
#include "../includes/kernel/mem/mem.h" 
#include "../includes/kernel/mem/pmm.h"
#include "../tests/libc_test.h"

#if DEBUG
//some debug code
#endif

extern "C" void load_gdt();
void print_memory_map(multiboot_info_t* mb_info);

extern "C" void kernel_main(multiboot_info_t* mb_info){
    load_gdt();  // Load our own GDT before anything else
    set_idt();
    init_exceptions();
    init_keyboard();
    //init_pmm();
    //init_mem();

    clean_screen();
    print_string((char *)"Welcome to OS from Scratch!\n", 28);
    print_string((char *)"Type 'help' for commands.\n", 26);
    print_string((char *)"os > ", 5);

    char buffer[256];
    
    for(;;){
        if (get_input_buffer(buffer, 256)) {
            if (strcmp(buffer, (char*)"help") == 0) {
                print_string((char*)"Available commands:\n", 20);
                print_string((char*)"  help  - Show this help\n", 25);
                print_string((char*)"  clear - Clear screen\n", 23);
                print_string((char*)"  echo  - Echo text\n", 20);
                print("blabla\n");
            } else if (strcmp(buffer, (char*)"clear") == 0) {
                clean_screen();
            } else if (buffer[0] == 'e' && buffer[1] == 'c' && buffer[2] == 'h' && buffer[3] == 'o' && buffer[4] == ' ') {
                print_string(buffer + 5, len(buffer + 5));
                print_string((char*)"\n", 1);
            } else if (len(buffer) > 0) {
                print_string((char*)"Unknown command: ", 17);
                print_string(buffer, len(buffer));
                print_string((char*)"\n", 1);
            }
            
            print_string((char *)"os > ", 5);
        }
    }
}

void print_memory_map(multiboot_info_t* mb_info){
    for(mmap_entry_t* entry = (mmap_entry_t*)mb_info->mmap_addr; entry < (mmap_entry_t*)(mb_info->mmap_addr + mb_info->mmap_length); entry++){
        print("base : ");
        char buffer[16];
        ptr_to_hex(entry->addr, buffer);
        print(buffer);
        print("\tlength : ");
        ptr_to_hex(entry->len, buffer);
        print(buffer);
        print("\ttype : ");
        print(int2String(entry->type));
        print("\n");
    }
}
