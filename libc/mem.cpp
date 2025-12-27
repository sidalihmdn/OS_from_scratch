#include <libc/mem.h>
#include <unit_types.h>

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

int memcmp(const void* ptr1, const void* ptr2, uint32_t size){
    const uint8_t* p1 = (const uint8_t*)ptr1;
    const uint8_t* p2 = (const uint8_t*)ptr2;
    for(uint32_t i = 0; i < size; i++){
        if (p1[i] != p2[i]){
            return (int)(p1[i]) - (int)(p2[i]);
        }
    }
    return 0;
}
