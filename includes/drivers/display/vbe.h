#ifndef VBE_H
#define VBE_H

#include "../../unit_types.h"
#include "../../boot/multiboot_helpers.h"

/*
 * Adresse virtuelle où on mappe le framebuffer
 */
#define VIDEO_VRT_ADDR 0xE0000000



void vbe_init(multiboot_info_t* mb_info);
void vbe_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void vbe_clear_screen(uint32_t color);

/*
 * Getters
 */
uint32_t vbe_get_width();
uint32_t vbe_get_height();

/*
 * Helper couleur
 */
uint32_t vbe_rgb(uint8_t r, uint8_t g, uint8_t b);

#endif