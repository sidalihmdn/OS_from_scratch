#include <arch/interrupt_controller.h>
#include <arch/x86/io/ports.h>
#include <unit_types.h>
#include "consts.h"


void arch_irq_controller_init(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t a1, a2;
    
    // Save masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
    
    // Start initialization sequence (in cascade mode)
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    // Set vector offsets
    outb(PIC1_DATA, master_offset);
    outb(PIC2_DATA, slave_offset);
    
    // Tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC1_DATA, 4);
    // Tell Slave PIC its cascade identity (0000 0010)
    outb(PIC2_DATA, 2);
    
    // Set 8086/88 (MCS-80/85) mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    
    // Restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void arch_irq_send_eoi(uint8_t irq) {
    // If IRQ came from slave PIC, send EOI to both PICs
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    
    // Always send EOI to master PIC
    outb(PIC1_COMMAND, PIC_EOI);
}

void arch_irq_enable(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    // Determine which PIC to use
    if (irq < 8) {
        port = PIC1_DATA;  // Master PIC
    } else {
        port = PIC2_DATA;  // Slave PIC
        irq -= 8;          // Adjust IRQ number for slave
    }
    
    // Read current mask, clear the bit for this IRQ, write back
    value = inb(port);
    value &= ~(1 << irq);  // Clear bit = enable IRQ
    outb(port, value);
}

void arch_irq_disable(uint8_t irq) {
    uint16_t port;
    uint8_t value;
    
    // Determine which PIC to use
    if (irq < 8) {
        port = PIC1_DATA;  // Master PIC
    } else {
        port = PIC2_DATA;  // Slave PIC
        irq -= 8;          // Adjust IRQ number for slave
    }
    
    // Read current mask, set the bit for this IRQ, write back
    value = inb(port);
    value |= (1 << irq);   // Set bit = disable IRQ
    outb(port, value);
}

void arch_load_idt(uintptr_t* idtr) {
    __asm__ volatile ("lidt %0" : : "m"(*idtr));
}
