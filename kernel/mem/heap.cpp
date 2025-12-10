#include "../../includes/kernel/mem/heap.h"
#include "../../includes/libc/string.h"
#include "../../includes/libc/log.h"
#include "../../includes/kernel/mem/vmm.h"
#include "../../includes/kernel/mem/pmm.h"
extern uint32_t kernel_end;

static uintptr_t heap_limit;

static heap_block_t* first_block = NULL;

void init_heap (){
    uintptr_t heap_start = PA_VA(pmm_alloc_page());
    uintptr_t heap_end = heap_start + 4096;


    first_block = (heap_block_t*)heap_start;
    first_block->size = 4096 - sizeof(heap_block_t) ;
    first_block->is_free = true;
    first_block->next = NULL;
    first_block->prev = NULL;
    heap_limit = heap_end;
}

void* kmalloc(uint32_t size){
    heap_block_t* current_block = first_block;
    heap_block_t* last_block = first_block;
    size = (size + 3) & ~3;
    if (size == 0){
        return 0;
    }
    while (current_block != NULL){
        if (current_block->is_free && current_block->size >= size){
            uint32_t remaining = current_block->size - size;
            if (remaining >= sizeof(heap_block_t)+4){
                current_block->is_free = false;
                current_block->size = size;
                heap_block_t* new_block = (heap_block_t*)((uintptr_t)current_block + sizeof(heap_block_t) + size);
                new_block->size = remaining - sizeof(heap_block_t);
                new_block->is_free = true;
                new_block->next = current_block->next;
                new_block->prev = current_block;
                current_block->next = new_block;
                return (void*)((uintptr_t)current_block + sizeof(heap_block_t));
            }else{
                current_block->is_free = false;
                return (void*)((uintptr_t)current_block + sizeof(heap_block_t));
            }
        }
        last_block = current_block;
        current_block = current_block->next;
    }
    current_block = (heap_block_t*)expand_heap(size, (heap_block_t*)last_block);
    if (!current_block){
        return 0;
    }
    return (void*)((uintptr_t)current_block + sizeof(heap_block_t));
}

void kfree(void* ptr){
    heap_block_t* block = (heap_block_t*)((uintptr_t)ptr - sizeof(heap_block_t));
    block->is_free = true;
    coalesce_free_blocks();
}

void* expand_heap(uintptr_t size, heap_block_t* last_block){
    uintptr_t needed_size = size + sizeof(heap_block_t);
    uint32_t needed_pages = (needed_size + 4095) / 4096;
    uintptr_t new_block_addr = heap_limit;

    if(!last_block){
        last_block = first_block;
    }
    for (uint32_t i = 0; i < needed_pages; i++){
        uintptr_t new_page = pmm_alloc_page();
        if (!new_page){
            return 0;
        }
        map_page(heap_limit, new_page);
        heap_limit += 4096;
    }
    heap_block_t* new_block = (heap_block_t*)new_block_addr;
    new_block->size = size;
    new_block->is_free = false;
    new_block->prev = last_block;
    new_block->next = NULL;
    last_block->next = new_block;
    // TODO : investigate what happens when the first and unique block is expanded
    // the size is set to 0    
    

    // if any remaining space, create a new free block
    uint32_t remaining = needed_pages*4096-needed_size;
    if (remaining > sizeof(heap_block_t)+4){
        heap_block_t* new_free_block = (heap_block_t*)((uintptr_t)new_block + needed_size);
        new_free_block-> size = remaining - sizeof(heap_block_t);
        new_free_block->next = NULL;
        new_free_block->is_free = true;
        new_free_block->prev = new_block;
        new_block->next = new_free_block;
    }
    return (void*)new_block;
}
void coalesce_free_blocks(){
    heap_block_t* current_block = first_block;
    while (current_block->next != NULL){
        if (current_block->is_free && current_block->next->is_free){
            current_block->size += current_block->next->size + sizeof(heap_block_t);
            current_block->next = current_block->next->next;
            if (current_block->next != NULL){
                current_block->next->prev = current_block;
            }
        }
        else{
            current_block = current_block->next;
        }
    }
}
    
void print_heap_info(){
    heap_block_t* current_block = first_block;
    printk("=== HEAP INFO ===\n");
    while (current_block != NULL){
        printk("Block at %s, size %d, free %d\n", 
            ptr_to_hex((uintptr_t)current_block), 
            current_block->size, 
            current_block->is_free);
        current_block = current_block->next;
    }
    printk("=== END HEAP INFO ===\n");
}

void print_heap_block(heap_block_t* block){
    printk("Block at %s, size %d, free %d\n", 
        ptr_to_hex((uintptr_t)block), 
        block->size, 
        block->is_free);
}
