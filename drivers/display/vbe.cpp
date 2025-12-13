#include <drivers/display/vbe.h>
#include <boot/multiboot_helpers.h>
#include <unit_types.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/heap.h>
#include <libc/log.h>
#include <libc/mem.h>

static uintptr_t* video_buffer;
static uint32_t size;
static multiboot_info_t* multiboot_info;

void vbe_init(multiboot_info_t* mb_info){
    multiboot_info = mb_info;
    uintptr_t framebuffer_addr = (uintptr_t)multiboot_info->framebuffer_addr;
    size = multiboot_info->framebuffer_pitch * multiboot_info->framebuffer_height;
    
    map_region(VIDEO_VRT_ADDR, framebuffer_addr, size);
    /*
    * allocate 3 time the size of the buffer so we can keep some history
    */
    video_buffer = (uintptr_t*)kmalloc(size); 
}

void vbe_put_pixel(uint32_t x, uint32_t y, uint32_t color){
    uint32_t offset = (y * multiboot_info->framebuffer_pitch) + (x * multiboot_info->framebuffer_bpp / 8);
    uintptr_t addr = (uintptr_t)video_buffer + offset;

    if (addr < (uintptr_t)video_buffer || addr >= (uintptr_t)video_buffer + size){
        LOG_ERROR("vbe_put_pixel: addr out of bounds");
        return;
    }
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

void vbe_scroll_up(uint32_t lines, uint32_t bg_color){
    uint32_t line_height = 8;
    uint32_t stride = multiboot_info->framebuffer_pitch;
    uint32_t block_size = lines * line_height * stride;

    uint32_t screen_size = multiboot_info->framebuffer_height * stride;
    uint32_t pixel_count = block_size / 4;
    uint32_t* buffer_ptr = (uint32_t*)((uint8_t*)video_buffer + screen_size - block_size);

    memcpy((uint8_t*)video_buffer,
        (uint8_t*)video_buffer + block_size,
        screen_size - block_size
    );
    for(uint32_t i = 0; i < pixel_count; i++){
        buffer_ptr[i] = bg_color;
    }
}

uint32_t vbe_rgb(uint8_t r, uint8_t g, uint8_t b){
    return (r << 16) | (g << 8) | b;
} 
    
void vbe_draw_line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color){
    for(uint32_t i = x1; i < x2; i++){
        vbe_put_pixel(i, y1, color);
    }
}

void vbe_draw_rect(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color){
    for(uint32_t i = x1; i < x2; i++){
        vbe_put_pixel(i, y1, color);
    }
}

void vbe_swap_buffer(){
    memcpy((void*)VIDEO_VRT_ADDR, (void*)video_buffer, size);
}
