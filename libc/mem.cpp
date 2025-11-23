#include "../includes/libc/mem.h"

// External symbols defined in the linker script (linker.ld)
// These mark the physical start and end of the heap memory area.
extern uint32_t heap_start;
extern uint32_t heap_end;

// Static variables to track heap state
static uint32_t heap_start_addr; // Physical address where heap starts
static uint32_t heap_end_addr;   // Physical address where heap ends
static uint32_t total_blocks;    // Total number of available memory blocks
static uint32_t bitmap_size;     // Size of the bitmap in bytes
static uint8_t* bitmap;          // Pointer to the start of the bitmap
static uint8_t* heap_data_start; // Pointer to where actual data storage begins (after bitmap)

// Initialize the memory manager
void init_mem(){
    // Get the addresses from the linker symbols
    heap_start_addr = (uint32_t)&heap_start;
    heap_end_addr = (uint32_t)&heap_end;

    // Calculate total size of the heap
    uint32_t total_heap_size = heap_end_addr - heap_start_addr;
    
    // Calculate how many blocks fit in the heap
    total_blocks = total_heap_size / BLOCK_SIZE;
    
    // Calculate size of the bitmap needed (1 bit per block -> 8 blocks per byte)
    bitmap_size = total_blocks / 8;
    
    // Place the bitmap at the very beginning of the heap
    bitmap = (uint8_t*)heap_start_addr;
    
    // The actual data area starts right after the bitmap
    heap_data_start = (uint8_t*)(heap_start_addr + bitmap_size);
    
    // Clear the bitmap (set all bits to 0, meaning all blocks are free)
    memset(bitmap, 0, bitmap_size);
}

// Helper to manipulate bitmap
static void set_bit(uint32_t bit) {
    bitmap[bit/8] |= (1 << (bit % 8));
}

static void clear_bit(uint32_t bit) {
    bitmap[bit/8] &= ~(1 << (bit % 8));
}

static bool get_bit(uint32_t bit) {
    return bitmap[bit/8] & (1 << (bit % 8));
}

// Allocate memory of a given size
void* malloc(uint32_t size){
    // TODO: Implement allocation logic
    // 1. Calculate blocks needed
    // 2. Scan bitmap for free blocks
    // 3. Mark blocks as used
    // 4. Return pointer
    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    void* ptr = scan_bitmap(blocks_needed);
    
    return ptr;
}


void* scan_bitmap(uint32_t blocks_needed){
    uint32_t free_streak = 0;
    uint32_t start_block = 0;

    for (uint32_t i = 0; i < total_blocks; i++){
        if ( i + blocks_needed > total_blocks){
            break;
        }
        if (!get_bit(i)){
            start_block = i;
            free_streak = 1;
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
                    set_bits(start_block, start_block+blocks_needed);
                    return (void*)(heap_data_start + start_block*BLOCK_SIZE);
                }
            }
        }
    }
    return 0;
}

void set_bits(uint32_t start, uint32_t end){
    for (uint32_t i = start; i < end; i++){
        set_bit(i);
    }
}
    

// Free allocated memory
void free(void* ptr){
    // TODO: Implement deallocation logic
    // 1. Calculate block index from ptr
    // 2. Clear bits in bitmap
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

// Move memory area (handles overlapping regions)
void* memmove(void* dest, const void* src, uint32_t size){
    uint8_t* p = (uint8_t*)dest;
    const uint8_t* q = (const uint8_t*)src;
    // Note: This implementation is identical to memcpy and doesn't handle overlap correctly yet.
    // To handle overlap, we should copy backwards if dest > src.
    for (uint32_t i = 0; i < size; i++) {
        p[i] = q[i];
    }
    return dest;
}

// Locate character in block of memory
void* memchr(const void* ptr, int value, uint32_t size){
    uint8_t* p = (uint8_t*)ptr;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] == value) {
            return (void*)(p + i);
        }
    }
    return 0; // NULL
}

// Compare two blocks of memory
// Returns pointer to first mismatch or NULL if equal (Non-standard return type)
void* memcmp(const void* ptr1, const void* ptr2, uint32_t size){
    const uint8_t* p = (const uint8_t*)ptr1;
    const uint8_t* q = (const uint8_t*)ptr2;
    for (uint32_t i = 0; i < size; i++) {
        if (p[i] != q[i]) {
            return (void*)(p + i);
        }
    }
    return 0; // NULL
}

