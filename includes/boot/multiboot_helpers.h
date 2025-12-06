#pragma once
#include "../unit_types.h"

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot_info_t;

typedef struct{
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

typedef struct {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) memory_region_t;

typedef void (*mmap_callback_t)(mmap_entry_t* entry, void* context);


bool multiboot_validate(multiboot_info_t* mb_info);
uint32_t multiboot_get_total_memory(multiboot_info_t* mb_info);
uint32_t multiboot_get_mmap_entry_count(multiboot_info_t* mb_info);
void multiboot_iterate_mmap(multiboot_info_t* mb_info, mmap_callback_t callback, void* context); 
uint32_t multiboot_get_usable_regions(multiboot_info_t* mb_info, memory_region_t* regions, uint32_t max_regions);
bool multiboot_is_memory_region_usable(mmap_entry_t* entry);
