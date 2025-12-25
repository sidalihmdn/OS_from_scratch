#include "simplefs_internals.h"
#include <drivers/blocks/block_device.h>

static superblock_t* sb;

int simplefs_init(block_device_t* device){
    sb = (superblock_t*)kmalloc(sizeof(superblock_t));
    device->read(device, 0, sb, sizeof(superblock_t));
    if (sb->magic != SIMPLEFS_MAGIC){
        return -1;
    }
    return 0;
}

superblock_t* simplefs_get_superblock(){
    return sb;
}