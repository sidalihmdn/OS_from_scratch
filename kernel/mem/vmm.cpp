#include "../../includes/unit_types.h"
#include "../../includes/kernel/mem/pages_struct.h"
#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/kernel/mem/vmm.h"
#include "../../includes/libc/string.h"

#define KERNEL_VIRTUAL_BASE 0xC1000000u // 3GB + 1MB
#define PAGE_SIZE 4096

// assembly helper functions
extern "C" void load_page_directory(uint32_t* page_directory);
extern "C" void enable_paging();

extern uint32_t kernel_end;
extern uint32_t kernel_start;

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void flush_tlb(uintptr_t virtual_address);


void init_vmm(){
    mset((void*)page_directory, 0, 4096);

    // identity map the kernel
    map_region(KERNEL_VIRTUAL_BASE, (uintptr_t)&kernel_start, (uintptr_t)&kernel_end - (uintptr_t)&kernel_start);
    // identity map the first 4MB
    map_region(0, 0, 4096*512);

    // identity map the VGA memory
    map_region(0xB8000, 0xB8000, 4096);

    load_page_directory(page_directory);
    enable_paging();
}

void map_page(uintptr_t virtual_address, uintptr_t physical_address){
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    uint32_t page_directory_entry = page_directory[page_directory_index];
    if (!(page_directory_entry & 0x1)){
        uintptr_t new_page_table = alloc_page_table();
        page_directory[page_directory_index] = new_page_table | 0x3;
        page_directory_entry = page_directory[page_directory_index];
    }

    uint32_t* page_table = (uint32_t*)(page_directory_entry & 0xFFFFF000);
    page_table[page_table_index] = (physical_address & 0xFFFFF000) | 0x3;
    flush_tlb(virtual_address);
}

void unmap_page(uintptr_t virtual_address){
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    uint32_t page_directory_entry = page_directory[page_directory_index];
    if (!(page_directory_entry & 0x1)){
        return;
    }
    uint32_t* page_table = (uint32_t*)(page_directory_entry & 0xFFFFF000);
    page_table[page_table_index] = 0;
    flush_tlb(virtual_address);
}

void map_region(uintptr_t virtual_address, uintptr_t physical_address, uint32_t size){
    for (uintptr_t i = virtual_address; i < virtual_address + size; i += PAGE_SIZE){
        map_page(i, physical_address);
        physical_address += PAGE_SIZE;
    }
}

uintptr_t get_physical_address(uintptr_t virtual_address){
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;
    uint32_t offset = virtual_address & 0xFFF;

    // get page directory entry
    uint32_t page_directory_entry = page_directory[page_directory_index];
    // check if the present bit is set
    if (!(page_directory_entry & 0x1)){
        return 0;
    }
    // get page table entry
    uint32_t* page_table = (uint32_t*)(page_directory_entry & 0xFFFFF000);
    uint32_t page_table_entry = page_table[page_table_index];
    // check if the present bit is set
    if (!(page_table_entry & 0x1)){
        return 0;
    }
    return (page_table_entry & 0xFFFFF000) | offset;
}

uintptr_t alloc_page_table(){
    uint32_t* page_table = (uint32_t*)pmm_alloc_page();
    if (!page_table){
        return 0;
    }
    mset((void*)page_table, 0, 4096);
    return (uintptr_t)page_table;
}

void flush_tlb(uintptr_t virtual_address){
    asm volatile("invlpg (%0)" : : "r"(virtual_address) : "memory");
}
