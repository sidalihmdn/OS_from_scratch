#include "../../includes/kernel/mem/mem.h"
#include "../../includes/libc/string.h"

// External symbols defined in the linker script (linker.ld)
extern uint32_t heap_start;
extern uint32_t heap_end;

// Static variables to track heap state
static uint32_t heap_start_addr; 
static uint32_t heap_end_addr;   
static uint32_t total_blocks;    
static uint32_t bitmap_size;     
static uint8_t* bitmap;          
static uint8_t* heap_data_start; 

// Initialize the memory manager
void init_mem(){
    heap_start_addr = (uint32_t)&heap_start;
    heap_end_addr = (uint32_t)&heap_end;

    total_blocks = (heap_end_addr - (uint32_t)heap_data_start)/BLOCK_SIZE;
    bitmap_size = (total_blocks + 7) / 8;
    bitmap = (uint8_t*)heap_start_addr;
    heap_data_start = (uint8_t*)(heap_start_addr + bitmap_size);
    memset(bitmap, 0, bitmap_size);
}
void* malloc(uint32_t size){
    uint32_t blocks_needed = (size + sizeof(uint32_t) + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint32_t start_block = scan_bitmap(blocks_needed);
    if (start_block == -1){
        return nullptr;
    }
    set_bits(start_block, start_block + blocks_needed);
    uint32_t* start_block_ptr = (uint32_t*)(heap_data_start + start_block*BLOCK_SIZE);
    reserve_memory(blocks_needed, start_block_ptr);
    
    return start_block_ptr + 1;
}

void free(void* ptr){
    uint32_t* header_ptr = (uint32_t*)ptr - 1;
    uint32_t blocks_nb = *header_ptr;
    memset(header_ptr, 0, blocks_nb * BLOCK_SIZE);
    uint32_t start_block = ((uint32_t)header_ptr - (uint32_t)heap_data_start) / BLOCK_SIZE;
    clear_bits(start_block, start_block + blocks_nb);
}

static void set_bit(uint32_t bit) {
    bitmap[bit/8] |= (1 << (bit % 8));
}

static void clear_bit(uint32_t bit) {
    bitmap[bit/8] &= ~(1 << (bit % 8));
}

static bool get_bit(uint32_t bit) {
    return bitmap[bit/8] & (1 << (bit % 8));
}


uint32_t scan_bitmap(uint32_t blocks_needed){
    uint32_t free_streak = 0;
    uint32_t start_block = 0;

    for (uint32_t i = 0; i < total_blocks; i++){
        if ( i + blocks_needed > total_blocks){
            break;
        }
        if (!get_bit(i)){
            start_block = i;
            for (uint32_t j = i; j < blocks_needed+i; j++){
                if (j >= total_blocks){
                    break; // reached the end of the heap
                }
                if (!get_bit(j)){
                    free_streak++;
                }
                else{
                    start_block = 0;
                    free_streak = 0;
                    break;
                }
                if (free_streak == blocks_needed){
                    return start_block;
                }
            }
        }
    }
    return -1;
}

void set_bits(uint32_t start, uint32_t end){
    for (uint32_t i = start; i < end; i++){
        set_bit(i);
    }
}

void clear_bits(uint32_t start, uint32_t end){
    for (uint32_t i = start; i < end; i++){
        clear_bit(i);
    }
}

void reserve_memory(uint32_t blocks_needed, uint32_t* start_block){
    *start_block = blocks_needed;
    memset(start_block + 1, 0, blocks_needed * BLOCK_SIZE);
}

// Fill memory with a constant byte value
void* memset(void* ptr, int value, uint32_t size){
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = value;
    }
    return ptr;
}

// Copy memory area
void* memcpy(void* dest, const void* src, uint32_t size){
    uint8_t* p = (uint8_t*)dest;
    const uint8_t* q = (const uint8_t*)src;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = q[i];
    }
    return dest;
}

// not working
void* memmove(void* dest, const void* src, uint32_t size){
    uint8_t* p = (uint8_t*)dest;
    const uint8_t* q = (const uint8_t*)src;
    for (uint32_t i = 0; i < size; i++) {
        p[i] = q[i];
    }
    return dest;
}


void* memchr(const void* ptr, int value, uint32_t size){
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] == value) {
            return (void*)(p + i);
        }
    }
    return 0; 
}

// compare memory
void* memcmp(const void* ptr1, const void* ptr2, uint32_t size){
    const uint8_t* p = (const uint8_t*)ptr1;
    const uint8_t* q = (const uint8_t*)ptr2;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] != q[i]) {
            return (void*)(p + i);
        }
    }
    return 0; 
}

// print bitmap
void print_bitmap(){
    print("\n Bitmap: \n");
    for (uint32_t i = 0; i < 50; i++){
        if (get_bit(i)){
           print("X");
        }
        else{
            print(".");
        }
    }
}

