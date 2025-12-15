#include <arch/cpu_control.h>
#include <unit_types.h>

void arch_enable_interrupts(void) {
    __asm__ volatile ("sti");
}

void arch_disable_interrupts(void) {
    __asm__ volatile ("cli");
}

uint32_t arch_get_fault_address(void) {
    uint32_t val;
    __asm__ volatile("mov %%cr2, %0" : "=r"(val));
    return val;
}

void arch_halt(void) {
    __asm__ volatile("hlt");
}

void arch_load_idt(idtr_32* idtr) {
    __asm__ volatile ("lidt %0" : : "m"(*idtr));
}
