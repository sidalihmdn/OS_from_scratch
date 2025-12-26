#ifndef HEAP_INTERNALS_H
#define HEAP_INTERNALS_H
#define BLOCK_SIZE 32
 
typedef struct heap_block_t {
    uint32_t size;
    bool is_free;
    struct heap_block_t* next;
    struct heap_block_t* prev;
} heap_block_t;

void* expand_heap(uintptr_t size, heap_block_t* last_block);
void coalesce_free_blocks();
void print_heap_block(heap_block_t* block);

#endif
