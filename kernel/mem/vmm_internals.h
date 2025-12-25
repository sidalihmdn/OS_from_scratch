#ifndef VMM_INTERNALS_H
#define VMM_INTERNALS_H

#define PAGE_SIZE 4096
#define VIDEO_VRT_ADDR 0xE0000000


void map_page(uintptr_t virtual_address, uintptr_t physical_address);
void unmap_page(uintptr_t virtual_address);
void map_region(uintptr_t virtual_address, uintptr_t physical_address, uint32_t size);
void print_vmm_info();
uintptr_t alloc_page_table();
#endif