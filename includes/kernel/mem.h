#ifndef _KERNEL_MEM_H
#define _KERNEL_MEM_H
#include <kernel/mem/heap.h>
#include <unit_types.h>

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif /* _KERNEL_MEM_H */