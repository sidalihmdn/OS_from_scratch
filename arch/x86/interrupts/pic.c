#include <arch/interrupt_controller.h>
#include <arch/x86/io/ports.h>
#include <unit_types.h>

// PIC hardware definitions (internal use only)
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

#define PIC_EOI         0x20  // End of Interrupt command

#define ICW1_ICW4       0x01  // ICW4 needed
#define ICW1_SINGLE     0x02  // Single (cascade) mode
#define ICW1_INTERVAL4  0x04  // Call address interval 4 (8)
#define ICW1_LEVEL      0x08  // Level triggered (edge) mode
#define ICW1_INIT       0x10  // Initialization

#define ICW4_8086       0x01  // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO       0x02  // Auto (normal) EOI
#define ICW4_BUF_SLAVE  0x08  // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C  // Buffered mode/master
#define ICW4_SFNM       0x10  // Special fully nested (not)

// ========== HAL Implementation ==========

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
