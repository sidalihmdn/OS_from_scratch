#ifndef SIMPLEFS_BITMAP_H
#define SIMPLEFS_BITMAP_H

#include <unit_types.h>

int bitmap_alloc(uint8_t* bitmap, uint32_t total_blocks);
void bitmap_free(uint8_t* bitmap, uint32_t block_num);
void bitmap_set(uint8_t* bitmap, uint32_t block_num);
int bitmap_test(uint8_t* bitmap, uint32_t block_num);

#endif /* SIMPLEFS_BITMAP_H */
