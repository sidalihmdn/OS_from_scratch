#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <unit_types.h>

typedef struct block_device_t block_device_t;
struct block_device_t {
    uint64_t block_count;
    uint32_t block_size;
    int (*read)(block_device_t* device, uint64_t block, void* buffer, uint32_t size);
    int (*write)(block_device_t* device, uint64_t block, void* buffer, uint32_t size);
    void* data;
};

void block_device_register(block_device_t* device);
block_device_t* block_device_get(uint8_t index);
uint8_t block_device_count();

#endif