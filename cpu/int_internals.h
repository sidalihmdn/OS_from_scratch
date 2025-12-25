#define INT_GATE_FLAG   0x8E   
#define TRAP_GATE_FLAG  0x8F

/// @brief Main interrupt handler called from assembly
extern "C" void isr_handler(registers_t regs);
extern "C" void irq_handler(registers_t regs);


struct idt_entry_32 {
    uint16_t isr_addr_low;        // low address of the isr
    uint16_t kernel_code_seg;     // the kernel code segement address
    uint8_t reservet;             // 8 bit set to 0  (reserved by the cpu)
    uint8_t attributes;           // the flags and the type
    uint16_t isr_addr_high;       // high address of the isr
} __attribute__ ((packed));

struct idt_frame{ 
    uint32_t eip;  
    uint32_t cs; 
    uint32_t eflags; 
    uint32_t sp; 
    uint32_t ss; 
}__attribute__ ((packed)); // set the idt and the idtr

/// @brief this function will enable interrupts
void enable_interrupts();

/// @brief this function will disable interrupts
void disable_interrupts();

/// @brief this function will initialise the idt descriptor
/// @param isr the address of the interrupt service routine
/// @param attribute the attributes of the interrupt ( task gate .. etc)
/// @param entry the entry of the interrupt in the idt
void init_idt_desc(void *isr , uint8_t attribute, uint8_t entry);

/// @brief this function will set the idt
void set_idt(void);