#ifndef HEAP_H
#define HEAP_H
#include <unit_types.h>
 
void init_heap();
void kfree(void* ptr);
void* kmalloc(uint32_t size);

void print_heap_info();
#endif


    