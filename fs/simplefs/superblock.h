#ifndef SIMPLEFS_SUPERBLOCK_H
#define SIMPLEFS_SUPERBLOCK_H
#include <unit_types.h>
#include <drivers/block_device.h>
#include "simplefs_internals.h"

int sfs_read_superblock(block_device_t* device, superblock_t* sb);
int sfs_write_superblock(block_device_t* device, superblock_t* sb);
int sfs_validate_superblock(superblock_t* sb);
#endif /* SIMPLEFS_SUPERBLOCK_H */