#include "../includes/libc/mem.h"
#include "../includes/unit_types.h"

void* memcpy(void* dest, const void* src, uint32_t size){
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for(uint32_t i = 0; i < size; i++){
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* dest, uint8_t value, uint32_t size){
    uint8_t* d = (uint8_t*)dest;
    for(uint32_t i = 0; i < size; i++){
        d[i] = value;
    }
    return dest;
}

