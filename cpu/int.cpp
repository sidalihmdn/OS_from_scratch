#include "../includes/cpu/int.h"
#include "../includes/cpu/pic.h"
#include "../includes/cpu/ports.h"
#include "../includes/drivers/screen.h"
#include "../includes/drivers/keyboard.h"
#include "../includes/libc/string.h"

#define IDT_SIZE 256

// IDT entries array
idt_entry_32 idt[IDT_SIZE];
idtr_32 idtr;

// Interrupt handlers
isr_t interrupt_handlers[256];

// External assembly ISR stub table
extern "C" void* isr_stub_table[];

void init_idt_desc(void *isr, uint8_t attribute, uint8_t entry) {
    uint32_t isr_addr = (uint32_t)isr;
    
    idt[entry].isr_addr_low = (uint16_t)(isr_addr & 0xFFFF);
    idt[entry].isr_addr_high = (uint16_t)((isr_addr >> 16) & 0xFFFF);
    idt[entry].kernel_code_seg = 0x08; // GDT kernel code segment selector
    idt[entry].reservet = 0;
    idt[entry].attributes = attribute;
}

void set_idt(void) {
    // Initialize IDT pointer
    idtr.limite = (uint16_t)(sizeof(idt_entry_32) * IDT_SIZE - 1);
    idtr.base = (uint32_t)&idt;
    
    // Initialize interrupt handlers array
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }

    // Remap the PIC
    PIC_remap(32, 40);
    
    // Install ISR handlers for CPU exceptions (0-31) and IRQs (32-47)
    for (uint8_t i = 0; i < 48; i++) {
        init_idt_desc(isr_stub_table[i], INT_GATE_FLAG, i);
    }
    
    // Load IDT
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    
    // Enable interrupts
    __asm__ volatile ("sti");  // Commented out for now - causes crash with GRUB
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

extern "C" void isr_handler(registers_t regs) {
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    } else {
        print_string((char*)"Unhandled Interrupt: ", 21);
        // TODO: Print interrupt number (need itoa)
        print_string((char*)"\n", 1);
        for(;;);
    }
}

extern "C" void irq_handler(registers_t regs) {
    // Send EOI to PIC
    if (regs.int_no >= 40) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);

    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
    // check for the keyboard interrupt
    if (regs.int_no == 33) {
        keyboard_handler();
    }
}

extern "C" void exception_handler(registers_t regs, const char* msg){
    print("Exception - interrupt number : ");
    print(int2String(regs.int_no));
    print("\n");
    print("Exception - error code : ");
    print(int2String(regs.err_code));
    print("\n");
    print(msg);
    print("\n");
    for(;;);   
}

void isr0_handler(registers_t regs){
    exception_handler(regs, "Divide By Zero");
}

void isr13_handler(registers_t regs){
    exception_handler(regs, "General Protection Fault");
}

uint32_t get_cr2() {
    uint32_t val;
    asm volatile("mov %%cr2, %0" : "=r"(val));
    return val;
}

void isr14_handler(registers_t regs){
    printk("PAGE FAULT! Address: %x\n", get_cr2());
    while(1);
}



void init_exceptions(){
    register_interrupt_handler(0, isr0_handler);
    register_interrupt_handler(13, isr13_handler);
    register_interrupt_handler(14, isr14_handler);
}
