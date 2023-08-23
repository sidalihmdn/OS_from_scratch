// this file contains the function declarations for interrup handling
// still learning about it
#ifndef INT_H
#define INT_H

#include "../includes/unit_types.h"
#include "../drivers/screen.h"

#define INT_GATE_FLAG   0x8E   
#define TRAP_GATE_FLAG  0x8F

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
}__attribute__ ((packed)); // set the idt and the idtr 

// exception handler
__attribute__((noreturn))
extern "C" void exception_handler(void);

// interrupt handler
__attribute__((noreturn))
void int_handeler(void);

/// @brief this function will initialise the idt descriptor
/// @param kernel_code_seg the adress of the code segement
/// @param offset the offset of the segement in the gdt
/// @param attribute the attributes of the interrupt ( task gate .. etc)
/// @param idt_descriptor the address of the idt descriptor
void init_idt_desc(void *isr , uint8_t attribute, uint8_t entry);

void set_idt(void);

#endif // !INT_H
