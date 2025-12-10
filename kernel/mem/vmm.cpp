#include "../../includes/unit_types.h"
#include "../../includes/boot/multiboot_helpers.h"
#include "../../includes/kernel/panic.h"
#include "../../includes/kernel/mem/pages_struct.h"
#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/kernel/mem/vmm.h"
#include "../../includes/libc/string.h"





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
    uint32_t offset = 0;

    /* initialize the page directory */
    mset((void*)page_directory, 0, 4096);

    /* identity map the first 1MB */
    map_region(0, 0, 1024*1024*4);

    /* note : the heap is mapped after the kernel
    KERNEL_VIRTUAL_BASE = 0xC1000000 */
    map_region(HIGH_MEMORY_BASE, (uintptr_t)&kernel_start, (uintptr_t)&kernel_end - (uintptr_t)&kernel_start);

    /* the  offset is used to keep track of the virtual address */
    offset += (uintptr_t)&kernel_end - (uintptr_t)&kernel_start;

    /* get the count of memory regions + the regions are stored in the regions array */
    memory_region_t regions[32];
    uint32_t region_count = multiboot_get_usable_regions(mb_info, regions, 32);
    
    /* map all memory regions to high memory */
    for (uint32_t i = 0; i < region_count; i++){
        map_region(HIGH_MEMORY_BASE + offset, regions[i].addr, regions[i].len);
        offset += regions[i].len;
    }

    /* identity map the VGA memory */
    map_region(0xB8000, 0xB8000, 4096);

    /* load the page directory and enable paging */
    load_page_directory(page_directory);
    enable_paging();
}

void map_page(uintptr_t virtual_address, uintptr_t physical_address){
    /* get the page directory index and page table index */
    uint32_t page_directory_index = virtual_address >> 22;

    /* get the page table index */
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    /* get the page directory entry */
    uint32_t page_directory_entry = page_directory[page_directory_index];

    /* check if the present bit is not set */
    if (!(page_directory_entry & 0x1)){
        /* allocate a new page table */
        uintptr_t new_page_table = alloc_page_table();
        if (!new_page_table){
            PANIC("Failed to allocate page table");
        }
        /* set the page directory entry 
        * 0x3 is the page table flags - 011b (present : 1, writable : 1, user : 0)
        */
        page_directory[page_directory_index] = new_page_table | 0x3;
        page_directory_entry = page_directory[page_directory_index];
    }

    /* get the page table address
    * 0xFFFFF000 is the page table mask - 11111111111111111111111111110000b
    */
    uint32_t* page_table = (uint32_t*)(page_directory_entry & 0xFFFFF000);
    /* set the page table entry 
    * 0x3 is the page table flags - 011b (present : 1, writable : 1, user : 0)
    */
    page_table[page_table_index] = (physical_address & 0xFFFFF000) | 0x3;
    flush_tlb(virtual_address);
}

void unmap_page(uintptr_t virtual_address){
    /* this function unmaps a page*/

    /* get the page directory index - the 10 bits  */
    uint32_t page_directory_index = virtual_address >> 22;
    /* get the page table index - the bit from 12 to 21 */
    uint32_t page_table_index = (virtual_address >> 12) & 0x3FF;

    /* get the page directory entry */
    uint32_t page_directory_entry = page_directory[page_directory_index];

    /* check if the present bit is not set */
    if (!(page_directory_entry & 0x1)){
        return;
    }

    /* get the page table address
    * 0xFFFFF000 is the page table mask - 11111111111111111111111111110000b
    */
    uint32_t* page_table = (uint32_t*)(page_directory_entry & 0xFFFFF000);
   
    /* set the page table entry to 0 */
    page_table[page_table_index] = 0;

    /* flush the TLB */
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
    /* allocate a page table */
    uint32_t* page_table = (uint32_t*)pmm_alloc_page();
    if (!page_table){
        return 0;
    }
    /* set the page table to 0 */
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
