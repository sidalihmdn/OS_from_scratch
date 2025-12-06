#include "../../includes/kernel/mem/pmm.h"
#include "../../includes/libc/string.h"
#include "../../includes/boot/multiboot_helpers.h"
#include "../../includes/kernel/mem/pages_struct.h"

#define PAGE_SIZE 4096

#define ALIGN_UP(addr, align) (((addr) + (align) - 1) & ~((align) - 1))

#define SET_BIT(idx) (bitmap[idx/32] |= (1 << (idx % 32)))
#define CLEAR_BIT(idx) (bitmap[idx/32] &= ~(1 << (idx % 32)))
#define GET_BIT(idx) (bitmap[idx/32] & (1 << (idx % 32)))


// bitmap
extern uint32_t kernel_end;
extern uint32_t kernel_start;
uint32_t* bitmap = (uint32_t*)ALIGN_UP(kernel_end, 4096);

static uint32_t total_memory;
static uint32_t max_frames;
static uint32_t bitmap_size;

void free_region(uint64_t addr, uint64_t size);
void reserve_region(uint64_t addr, uint64_t size);


void init_pmm(multiboot_info_t* mb_info){
    total_memory = multiboot_get_total_memory(mb_info);
    max_frames = total_memory / PAGE_SIZE;
    bitmap_size = ((max_frames + 31) / 32) * sizeof(uint32_t);
    
    mset(bitmap, 0xFF, bitmap_size); 

    memory_region_t regions[32];
    uint32_t region_count = multiboot_get_usable_regions(mb_info, regions, 32);
    
    for(uint32_t i = 0; i < region_count; i++){
        if(regions[i].type == 1){
            free_region(regions[i].addr, regions[i].len);
        }
    }
    reserve_region(0, PAGE_SIZE);
    reserve_region((uint64_t)kernel_start, kernel_end - kernel_start);
    reserve_region((uint64_t)mb_info->mmap_addr, mb_info->mmap_length);
    reserve_region((uint64_t)bitmap, bitmap_size);
}

uint32_t pmm_alloc_page(){
    
    for (uint32_t i = 0; i < max_frames/32; i++){ // scan the bitmap
        if (bitmap[i] != 0xFFFFFFFF){ // if the bitmap is not full
            for (uint32_t j = 0; j < 32; j++){
                if(!(bitmap[i] & (1 << j))){ // if the bit is not set
                    bitmap[i] |= (1 << j); // set the bit
                    return (i*32 + j)*PAGE_SIZE; // return the address
                }
            }
        }
    }
    return 0;
}

void pmm_free_page(uint32_t addr){
    if (addr == 0 || addr >= max_frames*PAGE_SIZE || addr % PAGE_SIZE != 0){
        return;
    }
    uint32_t index = addr / PAGE_SIZE; // get the index of the frame
    CLEAR_BIT(index); // clear the bit
}

void free_region(uint64_t addr, uint64_t size){
    uint64_t start = addr / PAGE_SIZE;
    uint64_t end = (addr + size) / PAGE_SIZE;
    for (uint64_t i = start; i < end; i++){
        CLEAR_BIT(i);
    }
}

void reserve_region(uint64_t addr, uint64_t size){
    uint64_t start = addr / PAGE_SIZE;
    uint64_t end = (addr + size) / PAGE_SIZE;
    for (uint64_t i = start; i < end; i++){
        SET_BIT(i);
    }
}

void mset(void* ptr, int value, uint32_t size){
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = value;
    }
}
