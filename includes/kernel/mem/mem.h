 #pragma once

 #include "../../unit_types.h"
 
 #define BLOCK_SIZE 32
 
 void init_mem();
 void* malloc(uint32_t size);
 void free(void* ptr);

 uint32_t scan_bitmap(uint32_t blocks_needed);
 void set_bits(uint32_t start, uint32_t end);
 void clear_bits(uint32_t start, uint32_t end);
 void reserve_memory(uint32_t blocks, uint32_t* start_block_ptr);
 void print_bitmap();

 void* memset(void* ptr, int value, uint32_t num);
 void* memcpy(void* dest, const void* src, uint32_t num);
 void* memmove(void* dest, const void* src, uint32_t num);
 void* memchr(const void* ptr, int value, uint32_t num);
 void* memcmp(const void* ptr1, const void* ptr2, uint32_t num);

 