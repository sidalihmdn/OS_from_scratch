#ifndef HEAP_H
#define HEAP_H
#include "../../unit_types.h"
 
#define BLOCK_SIZE 32
 
typedef struct heap_block_t {
    uint32_t size;
    bool is_free;
    struct heap_block_t* next;
    struct heap_block_t* prev;
};

void init_heap();
void* kmalloc(uint32_t size);
void kfree(void* ptr);
void* expand_heap(uintptr_t size, heap_block_t* last_block);
void coalesce_free_blocks();

void print_heap_info();
void print_heap_block(heap_block_t* block);
#endif


    