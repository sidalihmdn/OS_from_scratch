#ifndef PIC_H
#define PIC_H

// PIC (Programmable Interrupt Controller) definitions
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

/// @brief Remap the PIC to new interrupt offsets
/// @param offset1 New offset for master PIC (usually 32)
/// @param offset2 New offset for slave PIC (usually 40)
void PIC_remap(int offset1, int offset2);

/// @brief Send End of Interrupt signal to PIC
/// @param irq IRQ number
void PIC_sendEOI(unsigned char irq);

/// @brief Clear (enable/unmask) a specific IRQ line
/// @param irq IRQ number (0-15)
void PIC_clear_mask(unsigned char irq);

/// @brief Set (disable/mask) a specific IRQ line
/// @param irq IRQ number (0-15)
void PIC_set_mask(unsigned char irq);

#endif // PIC_H
