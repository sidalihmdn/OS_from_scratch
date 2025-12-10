 #pragma once
 #include "../../boot/multiboot_helpers.h"
 
void init_pmm(multiboot_info_t* mb_info);
uint32_t pmm_alloc_page();
void pmm_free_page(uint32_t addr);
void mset(void* ptr, int value, uint32_t size);
void pmm_reserve_region(uint64_t addr, uint64_t size);
