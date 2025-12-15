#ifndef ARCH_CPU_CONTROL_H
#define ARCH_CPU_CONTROL_H

#include <unit_types.h>

typedef struct idtr_32{ 
    uint16_t limite; 
    uint32_t base; 
}__attribute__ ((packed)) idtr_32; 

/**
 * @brief Enable interrupts
 */
void arch_enable_interrupts(void);

/**
 * @brief Disable interrupts
 */
void arch_disable_interrupts(void);

/**
 * @brief Get current page fault address (CR2 on x86)
 */
uint32_t arch_get_fault_address(void);

/**
 * @brief Halt the CPU
 */
void arch_halt(void);

/**
 * @brief Load the IDT
 * @param idtr Pointer to the IDTR structure
 */
void arch_load_idt(idtr_32* idtr);

#endif