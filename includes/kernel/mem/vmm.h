#pragma once
#include "pages_struct.h"

void init_vmm();
void map_page(uintptr_t virtual_address, uintptr_t physical_address);
void unmap_page(uintptr_t virtual_address);
void map_region(uintptr_t virtual_address, uintptr_t physical_address, uint32_t size);
uintptr_t get_physical_address(uintptr_t virtual_address);

uintptr_t alloc_page_table();