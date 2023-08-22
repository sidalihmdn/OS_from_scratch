// this file contains the function declarations for interrup handling
// still learning about it
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

idt_entry_32 idt[256]; 
idtr_32 idtr;

// exception handler 
__attribute__ ((noreturn))
extern "C" void exception_handler(void);
void exception_handler(){
  __asm__ volatile("cli; hlt"); 
}

/// @brief this function will initialise the idt descriptor
/// @param kernel_code_seg the adress of the code segement
/// @param offset the offset of the segement in the gdt
/// @param attribute the attributes of the interrupt ( task gate .. etc)
/// @param idt_descriptor the address of the idt descriptor
void init_idt_desc(void *isr , uint8_t attribute, idt_entry_32 *idt_descriptor);
void init_idt_desc(
    void *isr, 
    uint8_t attributes,
    idt_entry_32 *idt_descriptor){

    idt_descriptor->isr_addr_low    = (uint32_t)isr & 0xffff;            // low bits of the ISR addr
    idt_descriptor->kernel_code_seg = 0x08;                         // offset of the code 
    idt_descriptor->attributes      = attributes;                    // the flags
    idt_descriptor->isr_addr_high   = ((uint32_t)isr & 0xffff0000) >> 16;  // high bits of the ISR addr
}

// initialize the idtr
void init_idtr(idtr_32 *idtr){
    idtr->limite = (uint16_t)(256*64);      // number of enteies * size of the entry
    idtr->base   = (uint32_t)&idt;          //the address of the firt entry
}

extern void* isr_stub_table[];

void set_idt(void);
void set_idt(){
  //init the idtr
  init_idtr(&idtr);

  // set ISRs from 0-31 to handel exceptions
  for(uint8_t entry =0 ; entry < 32 ; entry++){
    init_idt_desc(isr_stub_table[entry],
                  INT_GATE_FLAG,
                  &idt[entry]);

  } 
  __asm__ volatile("lidt %0" : : "m"(idtr));
  __asm__ volatile("sti");

}
