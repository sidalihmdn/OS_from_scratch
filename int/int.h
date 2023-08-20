// this file contains the function declarations for interrup handling
// still learning about it
#include "../includes/unit_types.h"
#include "../drivers/screen.h"

typedef struct idt_entry_32 {
    uint16_t isr_addr_low;        // low address of the isr
    uint16_t kernel_code_seg;     // the kernel code segement address
    uint8_t reservet;             // 8 bit set to 0  (reserved by the cpu)
    uint8_t attributes;           // the flags and the type
    uint16_t isr_addr_high;       // high address of the isr
} __attribute__ ((packed));


typedef struct idtr_32{
    uint16_t limite;
    uint32_t base;
}__attribute__ ((packed));

typedef struct idt_frame{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t sp;
    uint32_t ss;
}__attribute__ ((packed));


// set the idt and the idtr

idt_entry_32 idt[256];
idtr_32 idtr;


// exeption handler functions
__attribute__((interrupt)) void expt_handler(idt_frame* idt_frame32){
    print_string("DEFAULT EXEPTION HANDLER - NO ERROR CODE",40);
}

__attribute__((interrupt)) void expt_handler(idt_frame* idr_frame32 , uint32_t error_code){
    print_string("DEFAULT EXEPTION HANDLER - ERROR CODE", 37);
}

// interrupt handler function
__attribute__((interrupt)) void int_handeler(idt_frame* idr_frame32){
    print_string("DEFAULT INTERRUPT HANDLER - ERROR CODE", 37);
}

/// @brief this function will initialise the idt
/// @param kernel_code_seg 
/// @param offset 
/// @param attribute 
/// @param idt_descriptor 
void init_idt_desc(uint16_t kernel_code_seg, uint32_t offset, uint16_t attribute,idt_entry_32 *idt_descriptor)
{
    idt_descriptor->isr_addr_low = (offset & 0xffff);
    idt_descriptor->kernel_code_seg = kernel_code_seg;
    idt_descriptor->attributes = attribute;
    idt_descriptor->isr_addr_high = (offset & 0xffff0000) >> 16;
    return;
}
