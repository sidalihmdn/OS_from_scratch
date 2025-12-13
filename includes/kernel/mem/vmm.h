#ifndef VMM_H
#define VMM_H
#include <kernel/mem/pages_struct.h>
#include <boot/multiboot_helpers.h>

#define HIGH_MEMORY_BASE 0xC0000000u
#define KERNEL_VIRTUAL_BASE (HIGH_MEMORY_BASE + 0x100000u) // 3GB + 1MB

#define PAGE_SIZE 4096

#define PA_VA(pa) ((pa) + HIGH_MEMORY_BASE)
#define VA_PA(va) ((va) - HIGH_MEMORY_BASE)

void init_vmm(multiboot_info_t* mb_info);
void map_page(uintptr_t virtual_address, uintptr_t physical_address);
void unmap_page(uintptr_t virtual_address);
void map_region(uintptr_t virtual_address, uintptr_t physical_address, uint32_t size);
uintptr_t get_physical_address(uintptr_t virtual_address);
void print_vmm_info();
uintptr_t alloc_page_table();

#endif
