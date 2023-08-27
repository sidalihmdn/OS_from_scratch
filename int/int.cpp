#include "int.h"
#include "../includes/string.h"
#include "pic.h"

idt_entry_32 idt[256];
idtr_32 idtr;
///
///
__attribute__((interrupt)) void exception_handler(idt_frame *frame) {
  zprint((char *)"Except");
}

__attribute__((interrupt)) void divid_except_handler(idt_frame *frame) {
  zprint("Divide Exception!\n");
  frame->eip++;
}

__attribute__((interrupt)) void int_handler(idt_frame *frame) {
  zprint("interrupt ! ");
}

void init_idt_desc(void *isr, uint8_t attributes, uint8_t entry) {
  idt_entry_32 *idt_descriptor = &idt[entry];
  idt_descriptor->isr_addr_low =
      (uint32_t)isr & 0xffff;              // low bits of the ISR addr
  idt_descriptor->kernel_code_seg = 0x08;  // offset of the code
  idt_descriptor->attributes = attributes; // the flags
  idt_descriptor->isr_addr_high =
      ((uint32_t)isr & 0xffff0000) >> 16; // high bits of the ISR addr
}

// initialize the idtr
void init_idtr(idtr_32 *idtr, uint32_t idt) {
  idtr->limite = (uint16_t)(256 * 64); // number of enteies * size of the entry
  idtr->base = idt;                    // the address of the firt entry
}

void set_idt() {

  // init the idtr
  init_idtr(&idtr, (uint32_t)&idt);

  // set ISRs from 0-31 to handel exceptions
  for (uint8_t entry = 0; entry < 32; entry++) {
    init_idt_desc((void *)exception_handler, INT_GATE_FLAG, entry);
  }

  // setting the IRQs
  for (uint8_t entry = 32; entry < 48; entry++) {
    init_idt_desc((void *)int_handler, INT_GATE_FLAG, entry);
  }
  // set a costume divide exception handler
  init_idt_desc((void *)divid_except_handler, INT_GATE_FLAG, 0);

  __asm__ volatile("lidt %0" : : "m"(idtr));
  __asm__ volatile("sti");
}
