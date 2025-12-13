#include <libc/string.h>
#include <drivers/screen.h>

static inline void hlt(){
    __asm__ __volatile__("hlt");
}

extern "C" void cli(); // defined in cpu/int.asm

void panic(const char* message){
    cli();
    clean_screen();
    print("PANIC: ");
    print(message);
    print("\n");

    while(1){
        hlt();
    }
}

void panic_at(uint32_t line, const char* file, const char* message){
    cli();
    clean_screen();
    print("PANIC AT: ");
    print(file);
    print(":");
    print(int2String(line));
    print("\n");
    print(message);
    print("\n");

    while(1){
        hlt();
    }
}