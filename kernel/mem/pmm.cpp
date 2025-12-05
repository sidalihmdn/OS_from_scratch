#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/libc/string.h"
#include "../../includes/kernel/mem/mem.h"

#define PAGE_SIZE 4096
#define MAX_FRAMES 1024

#define SET_BIT(idx) (bitmap[idx/32] |= (1 << (idx % 32)))
#define CLEAR_BIT(idx) (bitmap[idx/32] &= ~(1 << (idx % 32)))
#define GET_BIT(idx) (bitmap[idx/32] & (1 << (idx % 32)))

// assembly helper functions
extern "C" void load_page_directory(uint32_t* page_directory);
extern "C" void enable_paging();

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

// bitmap
uint32_t bitmap[MAX_FRAMES/32] __attribute__((aligned(4096))); // 1 bit for every frame in physical memory

void mset(void* ptr, int value, uint32_t size);


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
    mset(bitmap, 0, sizeof(bitmap)); // clear the bitmap ; all frames are free

    // marking the first 1MB of memory as used
    for(uint32_t i = 0; i < 256; i++){
        SET_BIT(i);
    }
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

uint32_t pmm_alloc_page(){
    
    for (uint32_t i = 0; i < MAX_FRAMES/32; i++){ // scan the bitmap
        if (bitmap[i] != 0xFFFFFFFF){ // if the bitmap is not full
            for (uint32_t j = 0; j < 32; j++){
                if(!(bitmap[i] & (1 << j))){ // if the bit is not set
                    bitmap[i] |= (1 << j); // set the bit
                    return (i*32 + j)*PAGE_SIZE; // return the address
                }
            }
        }
    }
    return -1;
}

void pmm_free_page(uint32_t addr){
    if (addr == 0 || addr >= MAX_FRAMES*PAGE_SIZE || addr % PAGE_SIZE != 0){
        return;
    }
    uint32_t index = addr / PAGE_SIZE; // get the index of the frame
    CLEAR_BIT(index); // clear the bit
}

void mset(void* ptr, int value, uint32_t size){
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = value;
    }
}
