#pragma once
#include "../includes/unit_types.h"

#define MASTER_PIC_CMD  0x20  // master pic data port
#define MASTER_PIC_DATA 0x21  // master pic commande port
#define SLAVE_PIC_CMD   0xA0  // slave pic data port
#define SLAVE_PIC_DATA  0xA1  // slave pic commande port

#define PIC_EOI         0x20  // end of interrupt commande
/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
 
#define ICW1_ICW4	      0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	      0x10		/* Initialization - required! */
 
#define ICW4_8086	      0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	      0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	      0x10		/* Special fully nested (not) */
 
#define PIC_READ_IRR    0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR    0x0b    /* OCW3 irq service next CMD read */

void send_EOI(uint8_t irq);
void PIC_remap(int offset1, int offset2);
void IRQ_set_mask(uint8_t IRQLine);
void IRQ_clear_mask(uint8_t IRQLine);




