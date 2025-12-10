#include "../../includes/unit_types.h"
#include "../../includes/kernel/mem/pages_struct.h"
#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/kernel/mem/vmm.h"
#include "../../includes/libc/string.h"
#include "../../includes/boot/multiboot_helpers.h"





// assembly helper functions
extern "C" void load_page_directory(uint32_t* page_directory);
extern "C" void enable_paging();

extern uint32_t kernel_end;
extern uint32_t kernel_start;
extern uint32_t heap_start;
extern uint32_t heap_end;

uint32_t page_directory[1024] __attribute__((aligned(4096)));

void flush_tlb(uintptr_t virtual_address);


void init_vmm(multiboot_info_t* mb_info){
    mset((void*)page_directory, 0, 4096);
    uint32_t offset = 0;

    map_region(0, 0, 1024*1024*4); // identity map the first 1MB

    /* note : the heap is mapped after the kernel
    KERNEL_VIRTUAL_BASE = 0xC1000000 */
    map_region(HIGH_MEMORY_BASE, (uintptr_t)&kernel_start, (uintptr_t)&kernel_end - (uintptr_t)&kernel_start);
    offset += (uintptr_t)&kernel_end - (uintptr_t)&kernel_start;

    /* map all memory usable regions to high memory*/
    memory_region_t regions[32];
    uint32_t region_count = multiboot_get_usable_regions(mb_info, regions, 32);
    for (uint32_t i = 0; i < region_count; i++){
        map_region(HIGH_MEMORY_BASE + offset, regions[i].addr, regions[i].len);
        offset += regions[i].len;
    }

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

void print_vmm_info(){
    printk("Mapped regions  :\n");
    for (int i = 0; i < 1024; i++){
        if (page_directory[i] & 0x1){
            printk("  %d: %x\n", i, page_directory[i]);
        }
    }
}
