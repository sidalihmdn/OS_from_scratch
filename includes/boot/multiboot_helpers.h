#ifndef MULTIBOOT_HELPERS_H
#define MULTIBOOT_HELPERS_H

#include <unit_types.h>

typedef struct {
    uint32_t flags;
    /* Basic Memory */
    uint32_t mem_lower;
    uint32_t mem_upper;
    /* Boot Device */
    uint32_t boot_device;
    /* Command Line */
    uint32_t cmdline;
    /* Modules */
    uint32_t mods_count;
    uint32_t mods_addr;
    /* Symbols */
    uint32_t syms[4];
    /* Memory Map */
    uint32_t mmap_length;
    uint32_t mmap_addr;
    /* Modules */
    uint32_t drivers_length;
    uint32_t drivers_addr;
    /* config table */
    uint32_t config_table;
    /* Boot Loader Name */
    uint32_t boot_loader_name;
    /* ACPI */
    uint32_t amp_table;
    /* VBE */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    /* Framebuffer */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[6];
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
} memory_region_t;

typedef void (*mmap_callback_t)(mmap_entry_t* entry, void* context);


bool multiboot_validate(multiboot_info_t* mb_info);
uint32_t multiboot_get_total_memory(multiboot_info_t* mb_info);
uint32_t multiboot_get_mmap_entry_count(multiboot_info_t* mb_info);
void multiboot_iterate_mmap(multiboot_info_t* mb_info, mmap_callback_t callback, void* context); 
uint32_t multiboot_get_usable_regions(multiboot_info_t* mb_info, memory_region_t* regions, uint32_t max_regions);
bool multiboot_is_memory_region_usable(mmap_entry_t* entry);
uint32_t multiboot_get_usable_memory(multiboot_info_t* mb_info);
void multiboot_print_mmap(multiboot_info_t* mb_info);

#endif
