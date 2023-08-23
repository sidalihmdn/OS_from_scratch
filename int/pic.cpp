#include "pic.h"
#include "../includes/unit_types.h"
#include "../io_functions/low_level.h"

// declarations 
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);
static uint16_t __pic_get_irq_reg(int ocw3);

// send and end of interrupt
void send_EOI(uint8_t irq){
  if (irq >= 8){
    outb(SLAVE_PIC_CMD,PIC_EOI);
  }
  outb(MASTER_PIC_CMD,PIC_EOI);
}

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(MASTER_PIC_DATA);                        // save masks
	a2 = inb(SLAVE_PIC_DATA);
 
	outb(MASTER_PIC_CMD, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(SLAVE_PIC_CMD, ICW1_INIT | ICW1_ICW4);
	outb(MASTER_PIC_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(SLAVE_PIC_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(MASTER_PIC_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(SLAVE_PIC_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(MASTER_PIC_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	outb(SLAVE_PIC_DATA, ICW4_8086);
 
	outb(MASTER_PIC_DATA, a1);   // restore saved masks.
	outb(SLAVE_PIC_DATA, a2);
}

// set a mask pn some interrupts
void IRQ_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = MASTER_PIC_DATA;
    } else {
        port = SLAVE_PIC_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
// clear a mask on some interrupts
void IRQ_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = MASTER_PIC_DATA;
    } else {
        port = SLAVE_PIC_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}


/* Helper func */
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(MASTER_PIC_CMD, ocw3);
    outb(SLAVE_PIC_CMD, ocw3);
    return (inb(SLAVE_PIC_CMD) << 8) | inb(MASTER_PIC_CMD);
}
 
/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}
 
/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
