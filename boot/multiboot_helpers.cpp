#include "../includes/boot/multiboot_helpers.h"
#include "../includes/kernel/panic.h"
#include "../includes/libc/string.h"

uint32_t multiboot_get_total_memory(multiboot_info_t* mb_info){
    return (mb_info->mem_upper + mb_info->mem_lower) * 1024;
}

void multiboot_iterate_mmap(multiboot_info_t* mb_info, mmap_callback_t callback, void* context){

    if(!mb_info || !callback){
        return;
    }
    uintptr_t current_addr = mb_info->mmap_addr;
    uintptr_t end_addr = current_addr + mb_info->mmap_length;
    while (current_addr < end_addr) {
        mmap_entry_t* entry = (mmap_entry_t*)current_addr;
        
        callback(entry, context);
        // 2. CRITICAL FIX: Advance the pointer by (size + 4 bytes for the size field itself)
        current_addr += entry->size + sizeof(uint32_t); 
    }
} 

bool multiboot_mmap_validate(multiboot_info_t* mb_info){
    if(!mb_info){
        panic("multiboot_mmap_validate: mb_info is null");
        return false;
    }
    if(!(mb_info->flags & 0x1)){
        panic("multiboot_mmap_validate: mem_lower & mem_upper are not valide");
        return false;
    }
    if(!(mb_info->flags & 0x4)){
        panic("multiboot_mmap_validate: mmap_addr is not valid");
        return false;
    }
    return true;
}

struct mmap_context {
    uint32_t count;
    memory_region_t* regions;
    uint32_t max_regions;
};

uint32_t multiboot_get_usable_regions(multiboot_info_t* mb_info, memory_region_t* regions, uint32_t max_regions){
    if(!mb_info || !regions){
        return 0;
    }
    mmap_context context = {0, regions, max_regions};
    multiboot_iterate_mmap(mb_info, [](mmap_entry_t* entry, void* context){
        mmap_context* ctx = (mmap_context*)context;
        if(multiboot_is_memory_region_usable(entry)){
            ctx->regions[ctx->count].addr = entry->addr;
            ctx->regions[ctx->count].len = entry->len;
            ctx->regions[ctx->count].type = entry->type;
            ctx->count++;
        }
    }, &context);
    regions = context.regions;
    return context.count;
}

bool multiboot_is_memory_region_usable(mmap_entry_t* entry){
    if(!entry){
        return false;
    }
    if(entry->type == 1){
        return true;
    }
    return false;
}

void multiboot_print_mmap(multiboot_info_t* mb_info){
    if(!mb_info){
        panic("multiboot_print_mmap: mb_info is null");
        return;
    }
    uintptr_t current_addr = mb_info->mmap_addr;
    uintptr_t end_addr = current_addr + mb_info->mmap_length;
    printk("mmap_addr : %x, mmap_length : %x\n", (uint32_t)mb_info->mmap_addr, (uint32_t)mb_info->mmap_length);
    while (current_addr < end_addr) {
        mmap_entry_t* entry = (mmap_entry_t*)current_addr;
        uint64_t addr = entry->addr;
        uint64_t len = entry->len;
        uint32_t type = entry->type;
        uint32_t size = entry->size;
        printk("addr : %x, len : %x, type : %d, size : %d\n", (uint32_t)addr, (uint32_t)len, type, size);
        
        // 2. CRITICAL FIX: Advance the pointer by (size + 4 bytes for the size field itself)
        current_addr += entry->size + sizeof(uint32_t); 
    }
}
static_assert(sizeof(mmap_entry_t) == 24, "mmap_entry_t size is not 24 bytes");
