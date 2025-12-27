#include <unit_types.h>
#include <errno.h>
#include "simplefs_internals.h"

int bitmap_alloc(uint8_t* bitmap, uint32_t total_blocks){
    uint32_t total_bytes = (total_blocks + 7) / 8;
    for (uint32_t i_byte = 0; i_byte < total_bytes; i_byte++){
        if (bitmap[i_byte] != 0xFF){
            for (uint8_t i_bit = 0; i_bit < 8; i_bit++){
                uint32_t block_num = i_byte * 8 + i_bit;
                if (block_num >= total_blocks){
                    return -1;
                }
                if ((bitmap[i_byte] & (1 << i_bit)) == 0){
                    bitmap[i_byte] |= (1 << i_bit);
                    return block_num;
                }
            }
        }
    }
    return -1;
}

void bitmap_free(uint8_t* bitmap, uint32_t block_num){
    uint32_t i_byte = block_num / 8;
    uint8_t i_bit = block_num % 8;
    bitmap[i_byte] &= ~(1 << i_bit);
}

void bitmap_set(uint8_t* bitmap, uint32_t block_num){
    uint32_t i_byte = block_num / 8;
    uint8_t i_bit = block_num % 8;
    bitmap[i_byte] |= (1 << i_bit);
}

int bitmap_test(uint8_t* bitmap, uint32_t block_num){
    uint32_t i_byte = block_num / 8;
    uint8_t i_bit = block_num % 8;
    return (bitmap[i_byte] & (1 << i_bit)) != 0;
}


    