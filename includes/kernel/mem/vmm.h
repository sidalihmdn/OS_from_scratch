#ifndef VMM_H
#define VMM_H
#include <kernel/mem/pages_struct.h>
#include <boot/multiboot_helpers.h>

#define HIGH_MEMORY_BASE 0xC0000000u
#define VA_PA(va) ((va) - HIGH_MEMORY_BASE)
#define PA_VA(pa) ((pa) + HIGH_MEMORY_BASE)

void init_vmm(multiboot_info_t* mb_info);
uintptr_t get_physical_address(uintptr_t virtual_address);

#endif
