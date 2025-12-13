#include "../../includes/drivers/display/vbe.h"
#include "../../includes/boot/multiboot_helpers.h"
#include "../../includes/unit_types.h"
#include "../../includes/kernel/mem/vmm.h"
#include "../../includes/libc/log.h"

static uint32_t video_buffer;
static multiboot_info_t* multiboot_info;

void vbe_init(multiboot_info_t* mb_info){
    multiboot_info = mb_info;
    uintptr_t framebuffer_addr = (uintptr_t)multiboot_info->framebuffer_addr;
    uint32_t size = multiboot_info->framebuffer_pitch * multiboot_info->framebuffer_height;
    
    map_region(VIDEO_VRT_ADDR, framebuffer_addr, size);
    video_buffer = (uint32_t)VIDEO_VRT_ADDR;
}

void vbe_put_pixel(uint32_t x, uint32_t y, uint32_t color){
    uint32_t offset = (y * multiboot_info->framebuffer_pitch) + (x * multiboot_info->framebuffer_bpp / 8);
    uintptr_t addr = VIDEO_VRT_ADDR + offset;
    *(uint32_t*)addr = color;
}

void vbe_clear_screen(uint32_t color){
    uint32_t height = vbe_get_height();
    uint32_t width = vbe_get_width();
    for(uint32_t i = 0; i < height * width; i++){
        vbe_put_pixel(i % width, i / width, color);
    }
}

uint32_t vbe_get_width(){
    return multiboot_info->framebuffer_width;
}

uint32_t vbe_get_height(){
    return multiboot_info->framebuffer_height;
}

uint32_t vbe_rgb(uint8_t r, uint8_t g, uint8_t b){
    return (r << 16) | (g << 8) | b;
} 
    

