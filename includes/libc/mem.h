 #pragma once

 #include "../unit_types.h"
 
 #define BLOCK_SIZE 32
 
 void init_mem();
 void* malloc(uint32_t size);
 void free(void* ptr);

 void* memset(void* ptr, int value, uint32_t num);
 void* memcpy(void* dest, const void* src, uint32_t num);
 void* memmove(void* dest, const void* src, uint32_t num);
 void* memchr(const void* ptr, int value, uint32_t num);
 void* memcmp(const void* ptr1, const void* ptr2, uint32_t num);

 