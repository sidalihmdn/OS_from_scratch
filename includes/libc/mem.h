#ifndef MEM_H
#define MEM_H

#include "../../includes/unit_types.h"

void* memcpy(void* dest, const void* src, uint32_t size);
void* memset(void* dest, uint8_t value, uint32_t size);

#endif