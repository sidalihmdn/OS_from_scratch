#ifndef SIMPLEFS_MKFS_H
#define SIMPLEFS_MKFS_H

#include <unit_types.h>
#include <drivers/block_device.h>

int mkfs(block_device_t *device, uint32_t block_size, uint32_t total_blocks);

#endif /* SIMPLEFS_MKFS_H */