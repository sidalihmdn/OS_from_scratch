#include "../includes/boot/multiboot_helpers.h"

uint32_t multiboot_get_total_memory(multiboot_info_t* mb_info){
    return (mb_info->mem_upper + mb_info->mem_lower) * 1024;
}


void multiboot_iterate_mmap(multiboot_info_t* mb_info, mmap_callback_t callback, void* context){

    if(!mb_info || !callback){
        return;
    }
    mmap_entry_t* entry = (mmap_entry_t*)mb_info->mmap_addr;
    for(entry; (uint8_t*)entry < (uint8_t*)mb_info->mmap_addr + mb_info->mmap_length; entry++){
        callback(entry, context);
    }

    
} 