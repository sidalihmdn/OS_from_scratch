#ifndef ARCH_INTERRUPT_CONTROLLER_H
#define ARCH_INTERRUPT_CONTROLLER_H

#include <unit_types.h>

/**
 * @brief Initialize the interrupt controller
 * @param master_offset Interrupt offset for master controller (usually 32)
 * @param slave_offset Interrupt offset for slave controller (usually 40)
 */
void arch_irq_controller_init(uint8_t master_offset, uint8_t slave_offset);

/**
 * @brief Enable a specific IRQ line
 * @param irq IRQ number (0-15)
 */
void arch_irq_enable(uint8_t irq);

/**
 * @brief Disable a specific IRQ line
 * @param irq IRQ number (0-15)
 */
void arch_irq_disable(uint8_t irq);

/**
 * @brief Send End of Interrupt signal
 * @param irq IRQ number that completed
 */
void arch_irq_send_eoi(uint8_t irq);

/**
 * @brief Load the IDT
 * @param idtr Pointer to the IDTR structure
 */
void arch_load_idt(uintptr_t* idtr);

#endif