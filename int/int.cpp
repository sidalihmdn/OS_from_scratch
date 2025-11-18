#include "int.h"
#include "pic.h"
#include "../io_functions/low_level.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"

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
    __asm__ volatile ("sti");
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
