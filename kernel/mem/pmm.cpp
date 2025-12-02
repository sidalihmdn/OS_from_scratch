#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/libc/string.h"
#include "../../includes/kernel/mem/mem.h"

#define PAGE_SIZE 4096

// assembly helper functions
extern "C" void load_page_directory(uint32_t* page_directory);
extern "C" void enable_paging();

// External symbols defined in the linker script (linker.ld)
extern uint32_t heap_start;
extern uint32_t heap_end;

// Static variables to track heap state
static uint32_t heap_start_addr; 
static uint32_t heap_end_addr;   
static uint32_t total_pages;    
static uint32_t bitmap_size;     
static uint8_t* bitmap;          
static uint8_t* heap_data_start;

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t write_through : 1;
    uint32_t cache_disabled : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} __attribute__((packed)) page_table_entry;

typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t write_through : 1;
    uint32_t cache_disabled : 1;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} __attribute__((packed)) page_directory_entry;

void init_pmm(){
    for(uint32_t i = 0; i < 1024; i++){
        // page_table[i].present = 1;
        // page_table[i].rw = 1;
        // page_table[i].frame = i;
        // this is the same as above
        page_table[i] = (i * 4096) | 3;
    }
    // page_directory[0].frame = page_table
    // page_directory[0].present = 1;
    // page_directory[0].rw = 1;
    page_directory[0] = ((uint32_t)page_table) | 3;

    for (uint32_t i = 1; i < 1024; i++){
        page_directory[i] = 2; // rw = 1 ; present = 0
    }
    load_page_directory(page_directory);
    enable_paging();
}
    
